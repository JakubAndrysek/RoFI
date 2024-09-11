#include <memory>
#include "nvs_flash.h"
#include "nvs.hpp"

void esp::EspNvs::initNvs() {
    esp_err_t ret = nvs_flash_init();
    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
      ESP_ERROR_CHECK( nvs_flash_erase() );
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
}

std::unique_ptr<esp::EspNvs> esp::EspNvs::open(const std::string& nsname) {
    nvs_handle_t handle;
    if(nsname.size() > 15) {
        throw esp::EspNvsException("Namespace name is too long");
    }

    auto err = nvs_open(nsname.c_str(), NVS_READWRITE, &handle);
    if(err != ESP_OK) {
        throw esp::EspNvsException("Failed to open NVS namespace " + nsname + ": "+ std::string(esp_err_to_name(err)));
    }
    return std::make_unique<EspNvs>(nsname, handle);
}

esp::EspNvs::EspNvs(const std::string& nsname, nvs_handle_t handle) : _nsname(nsname), _handle(handle) {
}

esp::EspNvs::~EspNvs() {
    nvs_close(_handle);
}

bool esp::EspNvs::erase(const std::string& name) {
    auto res = nvs_erase_key(_handle, name.c_str());
    if(res != ESP_OK && res != ESP_ERR_NVS_NOT_FOUND) {
		throw esp::EspNvsException("Unexpected error in EspNvsKeyValue::erase for " + name + ": " + std::string(esp_err_to_name(res)));
    }
    return res == ESP_OK;
}

void esp::EspNvs::setInt(const std::string& name, int64_t value) {
    auto res = nvs_set_i64(_handle, name.c_str(), value);
    if(res != ESP_OK) {
        throw esp::EspNvsException("Unexpected error in esp::EspNvs::setInt for " + name + ": " + std::string(esp_err_to_name(res)));
    }
}

void esp::EspNvs::setFloat(const std::string& name, float value) {
    auto res = nvs_set_u32(_handle, name.c_str(), *static_cast<uint32_t*>(static_cast<void*>(&value)));
    if(res != ESP_OK) {
        throw esp::EspNvsException("Unexpected error in esp::EspNvs::setInt for " + name + ": " + std::string(esp_err_to_name(res)));
    }
}

void esp::EspNvs::setString(const std::string& name, const std::string& value) {
    auto res = nvs_set_str(_handle, name.c_str(), value.c_str());
    if(res != ESP_OK) {
        throw esp::EspNvsException("Unexpected error in esp::EspNvs::setString for " + name + ": " + std::string(esp_err_to_name(res)));
    }
}

int64_t esp::EspNvs::getInt(const std::string& name, int64_t def_value) {
    auto res = nvs_get_i64(_handle, name.c_str(), &def_value);
    if(res != ESP_OK && res != ESP_ERR_NVS_NOT_FOUND) {
        throw esp::EspNvsException("Unexpected error in esp::EspNvs::getInt for " + name + ": " + std::string(esp_err_to_name(res)));
    }
    return def_value;
}

float esp::EspNvs::getFloat(const std::string& name, float def_value) {
    auto res = nvs_get_u32(_handle, name.c_str(), static_cast<uint32_t*>(static_cast<void*>(&def_value)));
    if(res != ESP_OK && res != ESP_ERR_NVS_NOT_FOUND) {
        throw esp::EspNvsException("Unexpected error in esp::EspNvs::getFloat for " + name + ": " + std::string(esp_err_to_name(res)));
    }
    return def_value;
}

std::string esp::EspNvs::getString(const std::string& name, std::string def_value) {
    size_t str_len = 0;

    auto err = nvs_get_str(_handle, name.c_str(), NULL, &str_len);
    if(err == ESP_ERR_NVS_NOT_FOUND) {
        return def_value;
    } else if(err != ESP_OK) {
        throw esp::EspNvsException("Unexpected error in esp::EspNvs::getString for " + name + ": " + std::string(esp_err_to_name(err)));
        // return def_value;
    }

    std::string result(str_len-1, '0');
    err = nvs_get_str(_handle, name.c_str(), result.data(), &str_len);
    if(err != ESP_OK) {
        throw esp::EspNvsException("Unexpected error in esp::EspNvs::getString for " + name + ": " + std::string(esp_err_to_name(err)));
        // return def_value;
    }
    return result;
}

esp::EspNvs::DataType esp::EspNvs::getType(const std::string& name) {
    DataType typ = DataType::NOT_FOUND;

    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find(NVS_DEFAULT_PART_NAME, _nsname.c_str(), NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);

        if(name == info.key) {
            switch(info.type) {
                case NVS_TYPE_I64: typ = DataType::INT64; break;
                case NVS_TYPE_U32: typ = DataType::FLOAT32; break;
                case NVS_TYPE_STR: typ = DataType::STRING; break;
                default: break;
            }
            break;
        }

        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);

    return typ;
}

std::vector<std::string> esp::EspNvs::listKeys() {
    std::vector<std::string> keys;
    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find(NVS_DEFAULT_PART_NAME, _nsname.c_str(), NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        keys.push_back(info.key);
        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);
    return keys;
}

bool esp::EspNvs::commit() {
    auto res = nvs_commit(_handle);
    if(res != ESP_OK) {
        throw esp::EspNvsException("Unexpected error in esp::EspNvs::commit: " + std::string(esp_err_to_name(res)));
    }
    return res == ESP_OK;
}
