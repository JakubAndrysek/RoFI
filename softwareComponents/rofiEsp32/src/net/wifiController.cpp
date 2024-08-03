#include <thread>

#include "wifiController.hpp"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi_ap_get_sta_list.h"

static const char *TAG = "vfs";


esp::EspWifiController::EspWifiController() :
        _mode(Mode::DISABLED),
        _staMode(StaMode::BEST_SIGNAL),
        _apFallback(true),
        _currentIp({}),
        _eventLoopOurs(false),
        _handler_wifi(nullptr),
        _handler_ip(nullptr),
        _netIf(nullptr, &esp_netif_destroy_default_wifi),
        _kvModifyTimer(nullptr)
{
    esp_timer_create_args_t args;
    args.callback = onKvModifiedTimer;
    args.arg = (void*)this;
    args.dispatch_method = ESP_TIMER_TASK;

    auto err = esp_timer_create(&args, &_kvModifyTimer);
    if(err != ESP_OK) {
        throw EspWifiControllerException("esp::EspWifiController esp_timer_create: " + std::string(esp_err_to_name(err)));
    }
}

bool esp::EspWifiController::initialize() {
    auto kvMain = EspNvs::open(KvNsWifiMain);
    if(!kvMain) {
        return false;
    }
    ESP_LOGD(TAG, "Initializing WiFi controller");

    _staMode = (StaMode)kvMain->getInt(KeyWifiStaMode, StaMode::BEST_SIGNAL);

    _apFallback = kvMain->getInt(KeyWifiStaApFallback, 1) != 0;

    return switchMode((Mode)kvMain->getInt(KeyWifiMode, Mode::DISABLED), std::move(kvMain));
}

bool esp::EspWifiController::initGlobalsLocked() {
    auto err = esp_netif_init();
    if(err != ESP_OK) {
        throw EspWifiControllerException("esp_netif_init: " + std::string(esp_err_to_name(err)));
        return false;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = 0;
    cfg.nano_enable = 1;

    err = esp_event_loop_create_default();
    _eventLoopOurs = (err == ESP_OK);
    if(err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        throw EspWifiControllerException("esp_event_loop_create_default: " + std::string(esp_err_to_name(err)));
        goto fail_netif;
    }

    err = esp_wifi_init(&cfg);
    if(err != ESP_OK) {
        throw EspWifiControllerException("esp_wifi_init: " + std::string(esp_err_to_name(err)));
        goto fail_netif;
    }
    return true;

fail_netif:
    if(_eventLoopOurs) {
        esp_event_loop_delete_default();
        _eventLoopOurs = false;
    }
    esp_netif_deinit();
    return false;
}

bool esp::EspWifiController::registerEventHandlersLocked() {
    auto err = esp_event_handler_instance_register(WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &esp::EspWifiController::eventHandlerWifi,
            (void*)this,
            &_handler_wifi);
    if(err != ESP_OK) {
        throw EspWifiControllerException("esp_event_handler_instance_register(WIFI_EVENT): " + std::string(esp_err_to_name(err)));
        return false;
    }

    err = esp_event_handler_instance_register(IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &esp::EspWifiController::eventHandlerIp,
            this,
            &_handler_ip);
    if(err != ESP_OK) {
        throw EspWifiControllerException("esp_event_handler_instance_register(IP_EVENT): " + std::string(esp_err_to_name(err)));
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, _handler_wifi);
        _handler_wifi = nullptr;
        return false;
    }
    return true;
}

void esp::EspWifiController::unregisterEventHandlersLocked() {
    if(_handler_wifi) {
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, _handler_wifi);
        _handler_wifi = nullptr;
    }
    if(_handler_ip) {
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, _handler_ip);
        _handler_ip = nullptr;
    }
}

void esp::EspWifiController::stopWifiLocked() {
    unregisterEventHandlersLocked();

    SemaphoreHandle_t stopSem = xSemaphoreCreateBinary();

    esp_event_handler_instance_t handler = nullptr;
    esp_event_handler_instance_register(WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &esp::EspWifiController::eventHandlerWifiStop,
            (void*)stopSem,
            &handler);

    if(esp_wifi_stop() == ESP_OK) {
        if(xSemaphoreTake(stopSem, pdMS_TO_TICKS(1000)) == pdFALSE) {
           throw EspWifiControllerException("Wait for Wifi Stop event timed out.");
        }
    }

    if(handler) {
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, handler);
    }
    vSemaphoreDelete(stopSem);

    _netIf.reset();

    _currentIp.addr = 0;
}

void esp::EspWifiController::deinitGlobalsLocked() {
    stopWifiLocked();
    esp_wifi_deinit();

    if(_eventLoopOurs) {
        esp_event_loop_delete_default();
        _eventLoopOurs = false;
    }

    esp_netif_deinit();
}


void esp::EspWifiController::onKeyValueModified(const std::string& nsname, const std::string& key) {
    if(nsname != KvNsWifiMain && nsname != KvNsWifiSsid) {
        return;
    }

    esp_timer_stop(_kvModifyTimer);

    auto err = esp_timer_start_once(_kvModifyTimer, 1000000);
    if(err != ESP_OK) {
        throw EspWifiControllerException("esp::EspWifiController esp_timer_start_once: " + std::string(esp_err_to_name(err)));
    }
}

void esp::EspWifiController::onKvModifiedTimer(void *selfVoid) {
    auto *self = (esp::EspWifiController*)selfVoid;

    ESP_LOGD(TAG, "Reloading WiFi settings from NVS");

    self->switchMode(Mode::DISABLED);
    self->initialize();
}

bool esp::EspWifiController::switchMode(Mode newMode, std::unique_ptr<esp::EspNvs> mainNs) {
    std::lock_guard<std::mutex> lg(_modeLock);

    if(newMode >= Mode::MAX) {
        newMode = Mode::DISABLED;
    }

    if(_mode == newMode) {
        return true;
    }

    ESP_LOGD(TAG, "Wifi mode switch from %d to %d", _mode, newMode);

    if(newMode == Mode::DISABLED) {
        deinitGlobalsLocked();
        _mode = newMode;
        return true;
    }

    if(!mainNs) {
        mainNs = EspNvs::open(KvNsWifiMain);
        if(!mainNs) {
            return false;
        }
    }

    if(_mode == Mode::DISABLED) {
        // Init for NetIf, Wifi Driver
        if(!initGlobalsLocked()) {
            return false;
        }
    } else {
        // stop previous mode
        stopWifiLocked();
    }

    _mode = newMode;

    switch(_mode) {
    case Mode::STATION: {
        _netIf.reset(esp_netif_create_default_wifi_sta());

        if(!registerEventHandlersLocked()) {
            return false;
        }

        auto err = esp_wifi_set_mode(WIFI_MODE_STA);
        if(err != ESP_OK) {
            throw EspWifiControllerException("esp_wifi_set_mode(WIFI_MODE_STA): " + std::string(esp_err_to_name(err)));
            return false;
        }

        wifi_config_t cfg = {};
        cfg.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;

        if(_staMode == StaMode::SPECIFIC_SSID) {
            auto wifiNs = EspNvs::open(KvNsWifiSsid);
            if(!wifiNs) {
                return false;
            }

            auto spec_ssid = mainNs->getString(KeyWifiStaSpecific);
            snprintf((char*)cfg.sta.ssid, 32, "%s", spec_ssid.c_str());
            snprintf((char*)cfg.sta.password, 64, "%s", wifiNs->getString(spec_ssid).c_str());
        }

        err = esp_wifi_set_config(WIFI_IF_STA, &cfg);
        if(err != ESP_OK) {
            throw EspWifiControllerException("esp_wifi_set_config(WIFI_IF_STA): " + std::string(esp_err_to_name(err)));
            return false;
        }
        break;
    }
    case Mode::AP: {
        _netIf.reset(esp_netif_create_default_wifi_ap());

        if(!registerEventHandlersLocked()) {
            return false;
        }

        auto err = esp_wifi_set_mode(WIFI_MODE_AP);
        if(err != ESP_OK) {
            throw EspWifiControllerException("esp_wifi_set_mode(WIFI_MODE_AP): " + std::string(esp_err_to_name(err)));
            return false;
        }

        const auto ssid = mainNs->getString(KeyWifiApSsid, "RoFI-AP");
        const auto pass = mainNs->getString(KeyWifiApPass);

        wifi_config_t cfg = {};
        if (pass.size() >= 8) {
            snprintf((char*)cfg.ap.password, 64, "%s", pass.c_str());
            cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
        } else {
            if(pass.size() != 0) {
                throw EspWifiControllerException("The WiFi password is too short, 8 characters required, leaving the WiFI open!");
            }
            cfg.ap.authmode = WIFI_AUTH_OPEN;
        }
        snprintf((char*)cfg.ap.ssid, 32, "%s", ssid.c_str());
        cfg.ap.beacon_interval = 400;
        cfg.ap.max_connection = 4;

        err = esp_wifi_set_config(WIFI_IF_AP, &cfg);
        if(err != ESP_OK) {
            throw EspWifiControllerException("esp_wifi_set_config: " + std::string(esp_err_to_name(err)));
            return false;
        }
        break;
    }
    default:
        break;
    }

    auto err = esp_wifi_start();
    if(err != ESP_OK) {
        throw EspWifiControllerException("esp_wifi_start: " + std::string(esp_err_to_name(err)));
        return false;
    }

    return true;
}

void esp::EspWifiController::eventHandlerWifi(void* selfVoid, esp_event_base_t event_base,
        int32_t event_id, void* event_data) {
    auto *self = (esp::EspWifiController*)selfVoid;

    switch(event_id) {
        case WIFI_EVENT_STA_START:
        case WIFI_EVENT_STA_DISCONNECTED: {
            std::lock_guard<std::mutex> lg(self->_modeLock);
            self->_currentStaSsid = "";

            if(self->_mode != Mode::STATION) {
                break;
            }

            if(self->_staMode == StaMode::SPECIFIC_SSID) {
                auto err = esp_wifi_connect();
                if(err != ESP_OK) {
                    throw EspWifiControllerException("esp_wifi_connect: " + std::string(esp_err_to_name(err)));
                }
            } else if(self->_staMode == StaMode::BEST_SIGNAL) {
                auto err = esp_wifi_scan_start(NULL, false);
                if(err != ESP_OK) {
                    throw EspWifiControllerException("esp_wifi_scan_start: " + std::string(esp_err_to_name(err)));
                }
            }
            break;
        }
        case WIFI_EVENT_AP_START: {
            std::lock_guard<std::mutex> lg(self->_modeLock);
            esp_netif_ip_info_t ip_info = {};
            esp_netif_get_ip_info(self->_netIf.get(), &ip_info);
            self->_currentIp = ip_info.ip;
            self->_currentStaSsid = "";
            break;
        }
        case WIFI_EVENT_SCAN_DONE: {
            std::lock_guard<std::mutex> lg(self->_modeLock);

            if(self->_mode != Mode::STATION || self->_staMode != StaMode::BEST_SIGNAL) {
                esp_wifi_clear_ap_list();
                break;
            }

            auto wifiNs = EspNvs::open(KvNsWifiSsid);
            if(!wifiNs) {
                esp_wifi_clear_ap_list();
                break;
            }

            wifi_ap_record_t rec;
            const std::string not_exists_sentinel = "a";
            bool connected = false;
            while(!connected) {
                auto err = esp_wifi_scan_get_ap_record(&rec);
                if(err != ESP_OK) {
                    break;
                }

                // kv key len limits us, use prefix match
                rec.ssid[15] = 0;
                auto pass = wifiNs->getString((const char*)rec.ssid, not_exists_sentinel);
                if(pass == not_exists_sentinel) {
                    continue;
                }

                wifi_config_t cfg = {};
                snprintf((char*)cfg.sta.ssid, 32, "%.31s", (const char*)rec.ssid);
                snprintf((char*)cfg.sta.password, 64, "%s", pass.c_str());
                err = esp_wifi_set_config(WIFI_IF_STA, &cfg);
                if(err != ESP_OK) {
                    throw EspWifiControllerException("esp_wifi_set_config: " + std::string(esp_err_to_name(err)));
                    continue;
                }

                err = esp_wifi_connect();
                if(err != ESP_OK) {
                    throw EspWifiControllerException("esp_wifi_connect: " + std::string(esp_err_to_name(err)));
                    continue;
                }

                self->_currentStaSsid = (const char*)rec.ssid;
                connected = true;
            }

            esp_wifi_clear_ap_list();

            if(!connected && self->_apFallback) {
                std::thread([self](){
                    self->switchMode(Mode::AP);
                }).detach();
            }
            break;
        }
    }
}

void esp::EspWifiController::eventHandlerWifiStop(void* semVoid, esp_event_base_t event_base,
        int32_t event_id, void* event_data) {
    switch(event_id) {
        case WIFI_EVENT_STA_STOP:
        case WIFI_EVENT_AP_STOP: {
            auto *sem = (SemaphoreHandle_t)semVoid;
            xSemaphoreGive(sem);
            break;
        }
    }
}

void esp::EspWifiController::eventHandlerIp(void* selfVoid, esp_event_base_t event_base,
        int32_t event_id, void* event_data) {
    auto *self = (esp::EspWifiController*)selfVoid;

    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;

    self->_currentIp = event->ip_info.ip;

    char buf[16];
    esp_ip4addr_ntoa(&event->ip_info.ip, buf, sizeof(buf));
    ESP_LOGD(TAG, "SYSTEM_EVENT_STA_GOT_IP: %s", buf);
}

/////////////////// Public API ///////////////////

std::string esp::EspWifiController::modeStr() {
    switch (_mode) {
        case Mode::DISABLED: return "DISABLED";
        case Mode::STATION: return "STATION";
        case Mode::AP: return "AP";
        default: return "UNKNOWN";
    }
}


void esp::EspWifiController::setMode(Mode newMode) {
    auto kvMain = EspNvs::open(KvNsWifiMain);
    if(!kvMain) {
        return;
    }

    kvMain->setInt(KeyWifiMode, newMode);
    onKeyValueModified(KvNsWifiMain, KeyWifiMode);
}


void esp::EspWifiController::addWifiNetwork(const std::string& ssid, const std::string& pass) {
    auto wifiNs = EspNvs::open(KvNsWifiSsid);
    if(!wifiNs) {
        return;
    }

    // maximum key length is 15
    if(ssid.size() > 15) {
        throw EspWifiControllerException("SSID is too long, maximum 15 characters allowed (NVS key length limit)");
    }

    wifiNs->setString(ssid, pass);
    onKeyValueModified(KvNsWifiSsid, ssid);
}

void esp::EspWifiController::removeWifiNetwork(const std::string& ssid) {
    auto wifiNs = EspNvs::open(KvNsWifiSsid);
    if(!wifiNs) {
        return;
    }

    wifiNs->erase(ssid);
    onKeyValueModified(KvNsWifiSsid, ssid);
}

std::vector<std::string> esp::EspWifiController::listWifiNetworks() {
    auto wifiNs = EspNvs::open(KvNsWifiSsid);
    if(!wifiNs) {
        return {};
    }

    return wifiNs->listKeys();
}

void esp::EspWifiController::setApWifiNetwork(const std::string& ssid, const std::string& pass) {
    auto kvMain = EspNvs::open(KvNsWifiMain);
    if(!kvMain) {
        return;
    }

    kvMain->setString(KeyWifiApSsid, ssid);
    kvMain->setString(KeyWifiApPass, pass);
    onKeyValueModified(KvNsWifiMain, KeyWifiApSsid);
    onKeyValueModified(KvNsWifiMain, KeyWifiApPass);
}

void esp::EspWifiController::setApFallback(bool fallback) {
    auto kvMain = EspNvs::open(KvNsWifiMain);
    if(!kvMain) {
        return;
    }

    kvMain->setInt(KeyWifiStaApFallback, fallback);
    onKeyValueModified(KvNsWifiMain, KeyWifiStaApFallback);
}


void esp::EspWifiController::setStaModeOption(StaMode mode) {
    auto kvMain = EspNvs::open(KvNsWifiMain);
    if(!kvMain) {
        return;
    }

    kvMain->setInt(KeyWifiStaMode, mode);
    onKeyValueModified(KvNsWifiMain, KeyWifiStaMode);
}


std::vector<esp_netif_pair_mac_ip_t> esp::EspWifiController::getConnectedClients() {
    std::vector<esp_netif_pair_mac_ip_t> clients;

    if (_mode != Mode::AP) {
        return clients;  // Return empty vector if not in AP mode
    }

    wifi_sta_list_t wifi_sta_list;
    wifi_sta_mac_ip_list_t netif_sta_list;

    memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
    memset(&netif_sta_list, 0, sizeof(netif_sta_list));

    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    esp_wifi_ap_get_sta_list_with_ip(&wifi_sta_list, &netif_sta_list);

    for (int i = 0; i < netif_sta_list.num; i++) {
        clients.push_back(netif_sta_list.sta[i]);
    }

    return clients;
}

std::vector<std::pair<std::string, std::string>> esp::EspWifiController::getConnectedClientsStr() {
    std::vector<std::pair<std::string, std::string>> clientsPair;
    auto clients = getConnectedClients();

    for (auto &client : clients) {
        char ip_addr[16];
        esp_ip4addr_ntoa(&client.ip, ip_addr, sizeof(ip_addr));
        // convert mac address to readable string
        std::string mac_str;
        for (int i = 0; i < 6; i++) {
            mac_str += std::to_string(client.mac[i]);
            if (i < 5) {
                mac_str += ":";
            }
        }
        clientsPair.push_back(std::make_pair(mac_str, std::string(ip_addr)));
    }

    return clientsPair;
}
