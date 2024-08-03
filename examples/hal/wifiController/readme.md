# WiFi Controller example

The example shows RoFI WiFi controller basic usage.


```bash
...
I (727) main_task: Calling app_main()
Saved Networks:
SSID: MyWiFi
SSID: MyWiFi2
End of saved networks

Starting wifi
Setting mode to AP
Waiting for IP
I (2877) wifi:wifi driver task: 3ffce58c, prio:23, stack:6144, core=0
I (2877) system_api: Base MAC address is not set
I (2877) system_api: read default base MAC address from EFUSE
I (2877) wifi:wifi firmware version: dbb7b72
I (2887) wifi:wifi certification version: v7.0
I (2887) wifi:config NVS flash: disabled
I (2887) wifi:config nano formating: enabled
I (2897) wifi:Init data frame dynamic rx buffer num: 32
I (2897) wifi:Init static rx mgmt buffer num: 5
I (2907) wifi:Init management short buffer num: 32
I (2907) wifi:Init dynamic tx buffer num: 32
I (2917) wifi:Init static rx buffer size: 1600
I (2917) wifi:Init static rx buffer num: 10
I (2917) wifi:Init dynamic rx buffer num: 32
I (2927) wifi_init: rx ba win: 6
I (2927) wifi_init: tcpip mbox: 32
I (2937) wifi_init: udp mbox: 6
I (2937) wifi_init: tcp mbox: 6
I (2937) wifi_init: tcp tx win: 5744
I (2947) wifi_init: tcp rx win: 5744
I (2947) wifi_init: tcp mss: 1440
I (2957) wifi_init: WiFi IRAM OP enabled
I (2957) wifi_init: WiFi RX IRAM OP enabled
I (3217) phy_init: phy_version 4791,2c4672b,Dec 20 2023,16:06:06
I (3307) wifi:mode : softAP (24:6f:28:9d:0e:6d)
I (3307) wifi:Total power save buffer number: 16
I (3307) wifi:Init max length of beacon: 752/752
I (3307) wifi:Init max length of beacon: 752/752
IP: 192.168.4.1

Connected clients:
 - no connected clients
End of connected clients (waiting 2s)

I (22787) wifi:new:<1,0>, old:<1,1>, ap:<1,1>, sta:<255,255>, prof:1
I (22787) wifi:station: 02:60:a5:65:83:73 join, AID=1, bgn, 20
I (22857) wifi:<ba-add>idx:2 (ifx:1, 02:60:a5:65:83:73), tid:0, ssn:0, winSize:64
I (23157) esp_netif_lwip: DHCP server assigned IP to a client, IP is: 192.168.4.2
Connected clients:
 - 2:96:165:101:131:115 - 192.168.4.2
End of connected clients (waiting 2s)

I (24087) esp_netif_lwip: DHCP server assigned IP to a client, IP is: 192.168.4.2
Connected clients:
 - 2:96:165:101:131:115 - 192.168.4.2
End of connected clients (waiting 2s)

I (26007) esp_netif_lwip: DHCP server assigned IP to a client, IP is: 192.168.4.2
I (27457) wifi:<ba-add>idx:3 (ifx:1, 02:60:a5:65:83:73), tid:7, ssn:0, winSize:64
Connected clients:
 - 2:96:160:101:130:112 - 192.168.4.2
End of connected clients (waiting 2s)
```