EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 5
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L rofi:ESP32-WROVER U6
U 1 1 5E084D17
P 2200 2000
F 0 "U6" H 2200 3375 50  0000 C CNN
F 1 "ESP32-WROVER" H 2200 3284 50  0000 C CNN
F 2 "rofi:ESP32-WROVER" H 2200 2950 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf" H 2200 2950 50  0001 C CNN
F 4 "356-ESP32-WROB(16MB)" H 2200 2000 50  0001 C CNN "#manf"
	1    2200 2000
	1    0    0    -1  
$EndComp
Text GLabel 1550 1900 0    50   Input ~ 0
ESP_EN
Text Notes 3850 2750 0    50   ~ 0
IO0: LOW => Bootloader
Text Notes 3850 2650 0    50   ~ 0
IO2: Floating/LOW => bootloader
Text Notes 3850 2550 0    50   ~ 0
IO15: LOW => Silence boot message
Text Notes 3850 2450 0    50   ~ 0
IO12: HIGH => 1.8V flash
Text GLabel 8800 3350 2    50   Input ~ 0
TXD0
Text GLabel 3450 2850 2    50   Input ~ 0
RXD0
NoConn ~ 1550 1300
NoConn ~ 1550 1400
NoConn ~ 1550 1500
NoConn ~ 1550 1600
NoConn ~ 1550 1700
NoConn ~ 1550 1200
Wire Wire Line
	1550 2850 1450 2850
Wire Wire Line
	1450 2850 1450 2950
Wire Wire Line
	1450 2950 1550 2950
Wire Wire Line
	1550 3050 1450 3050
Wire Wire Line
	1450 3050 1450 2950
Connection ~ 1450 2950
Wire Wire Line
	1550 3150 1450 3150
Wire Wire Line
	1450 3150 1450 3050
Connection ~ 1450 3050
Wire Wire Line
	1450 3150 1450 3250
Connection ~ 1450 3150
$Comp
L Device:C C40
U 1 1 5E136009
P 1250 2550
F 0 "C40" H 1365 2596 50  0000 L CNN
F 1 "470n" H 1365 2505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 1288 2400 50  0001 C CNN
F 3 "~" H 1250 2550 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 1250 2550 50  0001 C CNN "#manf"
	1    1250 2550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C39
U 1 1 5E1367F8
P 800 2550
F 0 "C39" H 915 2596 50  0000 L CNN
F 1 "22u" H 915 2505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 838 2400 50  0001 C CNN
F 3 "~" H 800 2550 50  0001 C CNN
F 4 "GRM21BR61C226ME44L" H 800 2550 50  0001 C CNN "#manf"
	1    800  2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 2400 1250 2400
Wire Wire Line
	1250 2400 800  2400
Connection ~ 1250 2400
Text GLabel 2850 2650 2    50   Input ~ 0
SPI_MISO_MOSI
Text GLabel 2850 1650 2    50   Input ~ 0
DOCK_1_CE
Text GLabel 2850 1450 2    50   Input ~ 0
DOCK_2_CE
Text GLabel 2850 1250 2    50   Input ~ 0
DOCK_3_CE
Text GLabel 2850 2150 2    50   Input ~ 0
DOCK_4_CE
Text GLabel 2850 1350 2    50   Input ~ 0
DOCK_5_CE
Text GLabel 2850 1550 2    50   Input ~ 0
DOCK_6_CE
Text GLabel 2850 2750 2    50   Input ~ 0
MOTORS_RX
Wire Wire Line
	2850 3150 2950 3150
Wire Wire Line
	2950 3150 2950 3700
Wire Wire Line
	2850 3050 3050 3050
Wire Wire Line
	3050 3050 3050 3550
Text GLabel 2850 1150 2    50   Input ~ 0
BIOS_TX
Text GLabel 2850 2450 2    50   Input ~ 0
BIOS_RX
Text GLabel 2850 2550 2    50   Input ~ 0
SPI_SCK
Text GLabel 4700 850  0    50   Input ~ 0
ESP_TMS
Text GLabel 4700 950  0    50   Input ~ 0
ESP_TCK
Text GLabel 4700 1050 0    50   Input ~ 0
ESP_TDO
Text GLabel 4700 1150 0    50   Input ~ 0
ESP_TDI
Text GLabel 2850 1750 2    50   Input ~ 0
ESP_TMS
Text GLabel 2850 2050 2    50   Input ~ 0
ESP_TDO
Text GLabel 2850 1850 2    50   Input ~ 0
ESP_TDI
Text GLabel 2850 1950 2    50   Input ~ 0
ESP_TCK
Connection ~ 3950 3550
Connection ~ 3950 3700
NoConn ~ 2850 850 
NoConn ~ 2850 950 
NoConn ~ 2850 1050
Text HLabel 4700 750  0    50   Input ~ 0
GND
Text HLabel 3950 3250 1    50   Input ~ 0
3V3
Text HLabel 3950 4000 3    50   Input ~ 0
3V3
Text HLabel 1450 3250 3    50   Input ~ 0
GND
Text HLabel 1250 2700 3    50   Input ~ 0
GND
Text HLabel 800  2700 3    50   Input ~ 0
GND
Text HLabel 800  2400 1    50   Input ~ 0
3V3
Text HLabel 7500 3150 0    50   Input ~ 0
SDA
Wire Wire Line
	2850 2950 3450 2950
Wire Wire Line
	2850 2850 3450 2850
Wire Wire Line
	3400 3550 3950 3550
Wire Wire Line
	3050 3550 3400 3550
Connection ~ 3400 3550
$Comp
L Connector:TestPoint TP3
U 1 1 5E8B6CC3
P 3400 3450
F 0 "TP3" H 3458 3568 50  0000 L CNN
F 1 "TP_SCL" H 3458 3477 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D2.0mm" H 3600 3450 50  0001 C CNN
F 3 "~" H 3600 3450 50  0001 C CNN
	1    3400 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 3450 3400 3550
Wire Wire Line
	3400 3700 3950 3700
Connection ~ 3400 3700
Wire Wire Line
	2950 3700 3400 3700
Wire Wire Line
	3400 3800 3400 3700
$Comp
L Connector:TestPoint TP4
U 1 1 5E8BA514
P 3400 3800
F 0 "TP4" H 3342 3826 50  0000 R CNN
F 1 "TP_SDA" H 3342 3917 50  0000 R CNN
F 2 "TestPoint:TestPoint_Pad_D2.0mm" H 3600 3800 50  0001 C CNN
F 3 "~" H 3600 3800 50  0001 C CNN
	1    3400 3800
	-1   0    0    1   
$EndComp
$Comp
L Device:R R29
U 1 1 5E6E28E0
P 3950 3850
F 0 "R29" H 4020 3896 50  0000 L CNN
F 1 "3k3" H 4020 3805 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3880 3850 50  0001 C CNN
F 3 "~" H 3950 3850 50  0001 C CNN
F 4 "RR0510P-332-D" H 3950 3850 50  0001 C CNN "#manf"
	1    3950 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R28
U 1 1 5E6E0D40
P 3950 3400
F 0 "R28" H 4020 3446 50  0000 L CNN
F 1 "3k3" H 4020 3355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3880 3400 50  0001 C CNN
F 3 "~" H 3950 3400 50  0001 C CNN
F 4 "RR0510P-332-D" H 3950 3400 50  0001 C CNN "#manf"
	1    3950 3400
	1    0    0    -1  
$EndComp
Text HLabel 4400 3700 2    50   Input ~ 0
SCL
Text HLabel 4400 3550 2    50   Input ~ 0
SDA
Text GLabel 2850 2350 2    50   Input ~ 0
MOTORS_TX
Text GLabel 8800 3450 2    50   Input ~ 0
USB_C_D-
Text GLabel 8800 3550 2    50   Input ~ 0
USB_C_D+
Text Label 8950 2350 0    50   ~ 0
BUS_VOLTAGE
Text Label 8950 2450 0    50   ~ 0
USB_VOLTAGE
Text HLabel 8800 2850 2    50   Input ~ 0
SW_LEFT
Text HLabel 8800 2950 2    50   Input ~ 0
SW_MID
Text HLabel 8800 3050 2    50   Input ~ 0
SW_RIGHT
Text HLabel 7500 3550 0    50   Output ~ 0
PWR_SHUTDOWN
$Comp
L Device:C C?
U 1 1 5FF817CD
P 8900 850
AR Path="/5E2C3773/5FF817CD" Ref="C?"  Part="1" 
AR Path="/5E080FD6/5FF817CD" Ref="C61"  Part="1" 
F 0 "C61" H 9015 896 50  0000 L CNN
F 1 "22u" H 9015 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8938 700 50  0001 C CNN
F 3 "~" H 8900 850 50  0001 C CNN
F 4 "GRM21BR61C226ME44L" H 8900 850 50  0001 C CNN "#manf"
	1    8900 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5FF817D4
P 9300 850
AR Path="/5E2C3773/5FF817D4" Ref="C?"  Part="1" 
AR Path="/5E080FD6/5FF817D4" Ref="C62"  Part="1" 
F 0 "C62" H 9415 896 50  0000 L CNN
F 1 "470n" H 9415 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 9338 700 50  0001 C CNN
F 3 "~" H 9300 850 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 9300 850 50  0001 C CNN "#manf"
	1    9300 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5FF817DB
P 9750 850
AR Path="/5E2C3773/5FF817DB" Ref="C?"  Part="1" 
AR Path="/5E080FD6/5FF817DB" Ref="C63"  Part="1" 
F 0 "C63" H 9865 896 50  0000 L CNN
F 1 "470n" H 9865 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 9788 700 50  0001 C CNN
F 3 "~" H 9750 850 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 9750 850 50  0001 C CNN "#manf"
	1    9750 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5FF817E2
P 10200 850
AR Path="/5E2C3773/5FF817E2" Ref="C?"  Part="1" 
AR Path="/5E080FD6/5FF817E2" Ref="C64"  Part="1" 
F 0 "C64" H 10315 896 50  0000 L CNN
F 1 "470n" H 10315 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 10238 700 50  0001 C CNN
F 3 "~" H 10200 850 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 10200 850 50  0001 C CNN "#manf"
	1    10200 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	10200 700  9750 700 
Wire Wire Line
	8900 700  9300 700 
Connection ~ 9750 700 
Connection ~ 9300 700 
Wire Wire Line
	9300 700  9750 700 
Connection ~ 8900 700 
Text HLabel 8900 1000 3    50   Input ~ 0
GND
Text HLabel 9300 1000 3    50   Input ~ 0
GND
Text HLabel 9750 1000 3    50   Input ~ 0
GND
Text HLabel 10200 1000 3    50   Input ~ 0
GND
Text HLabel 8000 4050 3    50   Input ~ 0
GND
Text GLabel 8800 3750 2    50   Input ~ 0
SWCLK
Text GLabel 8800 3650 2    50   Input ~ 0
SWDIO
Text GLabel 7700 5300 2    50   Input ~ 0
USB_C_D+
NoConn ~ 7700 4900
NoConn ~ 7700 4800
Wire Wire Line
	6800 6100 7100 6100
Wire Wire Line
	8000 4600 7950 4600
$Comp
L power:PWR_FLAG #FLG?
U 1 1 5FF8A755
P 7950 4600
AR Path="/5E2C3773/5FF8A755" Ref="#FLG?"  Part="1" 
AR Path="/5E080FD6/5FF8A755" Ref="#FLG0104"  Part="1" 
F 0 "#FLG0104" H 7950 4675 50  0001 C CNN
F 1 "PWR_FLAG" H 7950 4773 50  0000 C CNN
F 2 "" H 7950 4600 50  0001 C CNN
F 3 "~" H 7950 4600 50  0001 C CNN
	1    7950 4600
	1    0    0    -1  
$EndComp
Connection ~ 7950 4600
Wire Wire Line
	7950 4600 7700 4600
$Comp
L controlBoard-rescue:USB_C_Receptacle_USB2.0-Connector J?
U 1 1 5FF8A75D
P 7100 5200
AR Path="/5E2C3773/5FF8A75D" Ref="J?"  Part="1" 
AR Path="/5E080FD6/5FF8A75D" Ref="J8"  Part="1" 
AR Path="/5FF8A75D" Ref="J?"  Part="1" 
F 0 "J8" H 7207 6067 50  0000 C CNN
F 1 "USB_C_Receptacle_USB2.0" H 7207 5976 50  0000 C CNN
F 2 "rofi:USB_C_Female-16Pin-HPJF" H 7250 5200 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 7250 5200 50  0001 C CNN
	1    7100 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 5100 7700 5200
Wire Wire Line
	7700 5300 7700 5400
NoConn ~ 7700 5700
NoConn ~ 7700 5800
Text HLabel 7100 6100 3    50   Input ~ 0
GND
Text GLabel 7700 5100 2    50   Input ~ 0
USB_C_D-
$Comp
L Device:R R33
U 1 1 5FF9725F
P 9150 4850
AR Path="/5E080FD6/5FF9725F" Ref="R33"  Part="1" 
AR Path="/5E2C3773/5FF9725F" Ref="R?"  Part="1" 
F 0 "R33" H 9220 4896 50  0000 L CNN
F 1 "2k2" H 9220 4805 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 9080 4850 50  0001 C CNN
F 3 "~" H 9150 4850 50  0001 C CNN
F 4 "RR0510P-222-D" H 9150 4850 50  0001 C CNN "#manf"
	1    9150 4850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R35
U 1 1 5FF97266
P 9550 4850
AR Path="/5E080FD6/5FF97266" Ref="R35"  Part="1" 
AR Path="/5E2C3773/5FF97266" Ref="R?"  Part="1" 
F 0 "R35" H 9620 4896 50  0000 L CNN
F 1 "2k2" H 9620 4805 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 9480 4850 50  0001 C CNN
F 3 "~" H 9550 4850 50  0001 C CNN
F 4 "RR0510P-222-D" H 9550 4850 50  0001 C CNN "#manf"
	1    9550 4850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R34
U 1 1 5FF9726D
P 9150 5350
AR Path="/5E080FD6/5FF9726D" Ref="R34"  Part="1" 
AR Path="/5E2C3773/5FF9726D" Ref="R?"  Part="1" 
F 0 "R34" H 9220 5396 50  0000 L CNN
F 1 "10k" H 9220 5305 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 9080 5350 50  0001 C CNN
F 3 "~" H 9150 5350 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 9150 5350 50  0001 C CNN "#manf"
	1    9150 5350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R36
U 1 1 5FF97274
P 9550 5350
AR Path="/5E080FD6/5FF97274" Ref="R36"  Part="1" 
AR Path="/5E2C3773/5FF97274" Ref="R?"  Part="1" 
F 0 "R36" H 9620 5396 50  0000 L CNN
F 1 "10k" H 9620 5305 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 9480 5350 50  0001 C CNN
F 3 "~" H 9550 5350 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 9550 5350 50  0001 C CNN "#manf"
	1    9550 5350
	1    0    0    -1  
$EndComp
Text GLabel 9750 5100 2    50   Input ~ 0
USB_C_D+
Text GLabel 9050 5100 0    50   Input ~ 0
USB_C_D-
Wire Wire Line
	9550 5000 9550 5100
Wire Wire Line
	9550 5100 9750 5100
Connection ~ 9550 5100
Wire Wire Line
	9550 5100 9550 5200
Wire Wire Line
	9150 5000 9150 5100
Wire Wire Line
	9150 5100 9050 5100
Wire Wire Line
	9150 5100 9150 5200
Connection ~ 9150 5100
Wire Wire Line
	9150 4700 9150 4600
Wire Wire Line
	9150 4600 9100 4600
Wire Wire Line
	9550 4700 9550 4600
Wire Wire Line
	9550 4600 9600 4600
Wire Wire Line
	9550 5500 9550 5700
Wire Wire Line
	9550 5700 9600 5700
Wire Wire Line
	9150 5500 9150 5700
Wire Wire Line
	9150 5700 9100 5700
Text GLabel 7500 2450 0    50   Input ~ 0
QC_2_M
Text GLabel 9100 5700 0    50   Input ~ 0
QC_10_N
Text GLabel 9600 5700 2    50   Input ~ 0
QC_10_P
Text GLabel 9600 4600 2    50   Input ~ 0
QC_2_P
Text Notes 9100 4450 0    50   ~ 0
QuickCharge 3
Text Notes 6200 6500 0    50   ~ 0
https://github.com/Crypter/QC3Client/\nhttp://blog.deconinck.info/post/2017/08/09/Turning-a-Quick-Charge-3.0-charger-into-a-variable-voltage-power-supply
$Comp
L Device:R R?
U 1 1 5FF97293
P 4650 4550
AR Path="/5E2C3773/5FF97293" Ref="R?"  Part="1" 
AR Path="/5E080FD6/5FF97293" Ref="R23"  Part="1" 
F 0 "R23" H 4720 4596 50  0000 L CNN
F 1 "68k" H 4720 4505 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4580 4550 50  0001 C CNN
F 3 "~" H 4650 4550 50  0001 C CNN
F 4 "RR0510P-683-D" H 4650 4550 50  0001 C CNN "#manf"
	1    4650 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5FF9729A
P 4650 4950
AR Path="/5E2C3773/5FF9729A" Ref="R?"  Part="1" 
AR Path="/5E080FD6/5FF9729A" Ref="R30"  Part="1" 
F 0 "R30" H 4720 4996 50  0000 L CNN
F 1 "10k" H 4720 4905 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4580 4950 50  0001 C CNN
F 3 "~" H 4650 4950 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 4650 4950 50  0001 C CNN "#manf"
	1    4650 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 4700 4650 4750
Wire Wire Line
	4650 4750 5200 4750
Connection ~ 4650 4750
Wire Wire Line
	4650 4750 4650 4800
$Comp
L Device:R R?
U 1 1 5FF972A5
P 3600 4500
AR Path="/5E2C3773/5FF972A5" Ref="R?"  Part="1" 
AR Path="/5E080FD6/5FF972A5" Ref="R21"  Part="1" 
F 0 "R21" H 3670 4546 50  0000 L CNN
F 1 "68k" H 3670 4455 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3530 4500 50  0001 C CNN
F 3 "~" H 3600 4500 50  0001 C CNN
F 4 "RR0510P-683-D" H 3600 4500 50  0001 C CNN "#manf"
	1    3600 4500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5FF972AC
P 3600 4950
AR Path="/5E2C3773/5FF972AC" Ref="R?"  Part="1" 
AR Path="/5E080FD6/5FF972AC" Ref="R22"  Part="1" 
F 0 "R22" H 3670 4996 50  0000 L CNN
F 1 "10k" H 3670 4905 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3530 4950 50  0001 C CNN
F 3 "~" H 3600 4950 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 3600 4950 50  0001 C CNN "#manf"
	1    3600 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 4350 4650 4350
Wire Wire Line
	4650 4350 4650 4400
Wire Wire Line
	3350 4350 3600 4350
Wire Wire Line
	3600 4650 3600 4700
Wire Wire Line
	3950 4700 3600 4700
Connection ~ 3600 4700
Wire Wire Line
	3600 4700 3600 4800
Text GLabel 4700 1450 0    50   Input ~ 0
SWRST
Text GLabel 4700 1250 0    50   Input ~ 0
SWCLK
Text GLabel 4700 1350 0    50   Input ~ 0
SWDIO
Text HLabel 4700 1650 0    50   Input ~ 0
GND
Text HLabel 4700 1550 0    50   Input ~ 0
3V3
Text HLabel 3350 4350 0    50   Input ~ 0
INT
Text HLabel 4650 5100 3    50   Input ~ 0
GND
Text HLabel 3600 5100 3    50   Input ~ 0
GND
Text Label 4850 4750 0    50   ~ 0
USB_VOLTAGE
Text Label 3800 4700 0    50   ~ 0
BUS_VOLTAGE
Text GLabel 3450 2950 2    50   Input ~ 0
TXD0
Text GLabel 8800 3250 2    50   Input ~ 0
RXD0
Text GLabel 7500 2850 0    50   Input ~ 0
ESP_EN
Text GLabel 8800 2650 2    50   Input ~ 0
BIOS_RX
Text GLabel 8800 2550 2    50   Input ~ 0
BIOS_TX
$Comp
L Device:R R?
U 1 1 600E42B4
P 5600 4600
AR Path="/5DFADF1E/600E42B4" Ref="R?"  Part="1" 
AR Path="/5E2C3773/600E42B4" Ref="R?"  Part="1" 
AR Path="/5E080FD6/600E42B4" Ref="R31"  Part="1" 
F 0 "R31" H 5670 4646 50  0000 L CNN
F 1 "20k" H 5670 4555 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5530 4600 50  0001 C CNN
F 3 "~" H 5600 4600 50  0001 C CNN
F 4 "RR0510P-203-D" H 5600 4600 50  0001 C CNN "#manf"
F 5 "C25765" H 5600 4600 50  0001 C CNN "LCSC"
	1    5600 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 600E42BC
P 5600 4900
AR Path="/5DFADF1E/600E42BC" Ref="R?"  Part="1" 
AR Path="/5E2C3773/600E42BC" Ref="R?"  Part="1" 
AR Path="/5E080FD6/600E42BC" Ref="R32"  Part="1" 
F 0 "R32" H 5670 4946 50  0000 L CNN
F 1 "10k" H 5670 4855 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5530 4900 50  0001 C CNN
F 3 "~" H 5600 4900 50  0001 C CNN
F 4 "RT0402FRE0710KL" H 5600 4900 50  0001 C CNN "#manf"
F 5 "C25744" H 5600 4900 50  0001 C CNN "LCSC"
	1    5600 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 4750 5900 4750
Connection ~ 5600 4750
$Comp
L Connector:TestPoint TP?
U 1 1 600E42C4
P 5600 4300
AR Path="/5DFADF1E/600E42C4" Ref="TP?"  Part="1" 
AR Path="/5E2C3773/600E42C4" Ref="TP?"  Part="1" 
AR Path="/5E080FD6/600E42C4" Ref="TP1"  Part="1" 
F 0 "TP1" H 5658 4418 50  0000 L CNN
F 1 "TP_BAT" H 5658 4327 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D2.0mm" H 5800 4300 50  0001 C CNN
F 3 "~" H 5800 4300 50  0001 C CNN
	1    5600 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 4300 5600 4300
Wire Wire Line
	5600 4300 5600 4450
Connection ~ 5600 4300
Text HLabel 5500 4300 0    50   Input ~ 0
BATT_VDD
Text HLabel 5600 5050 3    50   UnSpc ~ 0
GND
Text Label 5900 4750 0    50   ~ 0
BATT_VOLTAGE
Text Label 9200 2750 0    50   ~ 0
BATT_VOLTAGE
$Comp
L MCU_ST_STM32F4:STM32F411CEUx U11
U 1 1 60D6650F
P 8200 2450
F 0 "U11" H 7650 4050 50  0000 C CNN
F 1 "STM32F411CEUx" H 7550 3950 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-48-1EP_7x7mm_P0.5mm_EP5.6x5.6mm" H 7600 950 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00115249.pdf" H 8200 2450 50  0001 C CNN
F 4 "C60420" H 8200 2450 50  0001 C CNN "LCSC"
	1    8200 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 700  8100 950 
Wire Wire Line
	8200 950  8200 700 
Connection ~ 8200 700 
Wire Wire Line
	8200 700  8300 700 
Wire Wire Line
	8300 950  8300 700 
Connection ~ 8300 700 
Wire Wire Line
	8000 700  8000 950 
Wire Wire Line
	8000 700  8100 700 
Connection ~ 8100 700 
Wire Wire Line
	8100 700  8200 700 
Text HLabel 5850 2350 3    50   Input ~ 0
GND
Text HLabel 8000 700  0    50   Input ~ 0
3V3
Text HLabel 5850 1950 1    50   Input ~ 0
3V3
$Comp
L Jumper:SolderJumper_3_Open JP1
U 1 1 60DC86B5
P 5850 2150
F 0 "JP1" V 5896 2218 50  0000 L CNN
F 1 "BootSelector" V 5805 2218 50  0000 L CNN
F 2 "Jumper:SolderJumper-3_P2.0mm_Open_TrianglePad1.0x1.5mm" H 5850 2150 50  0001 C CNN
F 3 "~" H 5850 2150 50  0001 C CNN
	1    5850 2150
	0    -1   -1   0   
$EndComp
Text HLabel 5850 1450 3    50   Input ~ 0
GND
Text GLabel 5800 1150 0    50   Input ~ 0
SWRST
$Comp
L Device:C C60
U 1 1 60DD73EA
P 6450 1700
F 0 "C60" H 6565 1746 50  0000 L CNN
F 1 "10u" H 6565 1655 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6488 1550 50  0001 C CNN
F 3 "~" H 6450 1700 50  0001 C CNN
F 4 "C15525" H 6450 1700 50  0001 C CNN "LCSC"
	1    6450 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 60DDACF0
P 5850 1300
AR Path="/5E2C3773/60DDACF0" Ref="C?"  Part="1" 
AR Path="/5E080FD6/60DDACF0" Ref="C59"  Part="1" 
F 0 "C59" H 5965 1346 50  0000 L CNN
F 1 "470n" H 5965 1255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 5888 1150 50  0001 C CNN
F 3 "~" H 5850 1300 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 5850 1300 50  0001 C CNN "#manf"
	1    5850 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 1150 5850 1150
Connection ~ 5850 1150
Text HLabel 6450 1850 3    50   Input ~ 0
GND
Wire Wire Line
	8800 2350 8950 2350
Wire Wire Line
	8950 2450 8800 2450
Wire Wire Line
	9200 2750 8800 2750
Text HLabel 8100 4050 3    50   Input ~ 0
GND
Text HLabel 8200 4050 3    50   Input ~ 0
GND
Text HLabel 8300 4050 3    50   Input ~ 0
GND
Text HLabel 8400 4050 3    50   Input ~ 0
GND
Text HLabel 7500 3250 0    50   Input ~ 0
SCL
Text GLabel 9100 4600 0    50   Input ~ 0
QC_2_M
Text GLabel 7500 2550 0    50   Input ~ 0
QC_10_N
Text GLabel 7500 2650 0    50   Input ~ 0
QC_2_P
Text GLabel 7500 2750 0    50   Input ~ 0
QC_10_P
Text HLabel 7500 3350 0    50   Input ~ 0
CHK_OK
Text HLabel 7500 3450 0    50   Output ~ 0
CHG_EN
Text GLabel 2850 2250 2    50   Input ~ 0
ESP_BOOT
Text GLabel 7500 2950 0    50   Input ~ 0
ESP_BOOT
Wire Wire Line
	3950 3550 4400 3550
Wire Wire Line
	3950 3700 4400 3700
$Comp
L Connector_Generic:Conn_01x20 J3
U 1 1 61066023
P 5450 6650
F 0 "J3" H 5368 7767 50  0000 C CNN
F 1 "ROFICOMS" H 5368 7676 50  0000 C CNN
F 2 "Connector_FFC-FPC:Hirose_FH12-20S-0.5SH_1x20-1MP_P0.50mm_Horizontal" H 5450 6650 50  0001 C CNN
F 3 "~" H 5450 6650 50  0001 C CNN
F 4 "C11055" H 5450 6650 50  0001 C CNN "LCSC"
	1    5450 6650
	-1   0    0    -1  
$EndComp
Text HLabel 5650 5750 2    50   Input ~ 0
GND
Text GLabel 5650 5850 2    50   Input ~ 0
SPI_SCK
Text HLabel 5650 5950 2    50   Input ~ 0
GND
Text GLabel 5650 6050 2    50   Input ~ 0
SPI_MISO_MOSI
Text HLabel 5650 6150 2    50   Input ~ 0
GND
Text GLabel 5650 6250 2    50   Input ~ 0
DOCK_1_CE
Text GLabel 5650 6350 2    50   Input ~ 0
DOCK_2_CE
Text GLabel 5650 6450 2    50   Input ~ 0
DOCK_3_CE
Text HLabel 5650 6650 2    50   Input ~ 0
BATT_VDD
Text HLabel 5650 6550 2    50   Input ~ 0
GND
Text HLabel 5650 6750 2    50   Input ~ 0
BATT_VDD
Text HLabel 5650 7050 2    50   Input ~ 0
INT
Text HLabel 5650 6850 2    50   Input ~ 0
GND
Text HLabel 5650 7150 2    50   Input ~ 0
INT
Text Label 5650 7350 0    50   ~ 0
EXT
Text Label 5650 7450 0    50   ~ 0
EXT
Text HLabel 5650 6950 2    50   Input ~ 0
GND
Text HLabel 5650 7250 2    50   Input ~ 0
GND
Text HLabel 5650 7550 2    50   Input ~ 0
GND
Text HLabel 5650 7650 2    50   Input ~ 0
GND
Text HLabel 8800 3150 2    50   Output ~ 0
BUZZER_P
Text HLabel 7500 3650 0    50   Output ~ 0
BUZZER_N
$Comp
L Sensor_Motion:MPU-6050 U7
U 1 1 60E649D6
P 1800 4650
F 0 "U7" H 2150 5200 50  0000 C CNN
F 1 "MPU-6050" H 1350 5200 50  0000 C CNN
F 2 "Sensor_Motion:InvenSense_QFN-24_4x4mm_P0.5mm" H 1800 3850 50  0001 C CNN
F 3 "https://store.invensense.com/datasheets/invensense/MPU-6050_DataSheet_V3%204.pdf" H 1800 4500 50  0001 C CNN
F 4 "C24112" H 1800 4650 50  0001 C CNN "LCSC"
	1    1800 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 4850 1000 4850
Wire Wire Line
	1000 4850 1000 4950
Wire Wire Line
	1000 4950 1100 4950
NoConn ~ 2500 4550
NoConn ~ 2500 4650
$Comp
L Device:C C42
U 1 1 60E75159
P 2700 4100
F 0 "C42" H 2815 4146 50  0000 L CNN
F 1 "470n" H 2815 4055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2738 3950 50  0001 C CNN
F 3 "~" H 2700 4100 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 2700 4100 50  0001 C CNN "#manf"
	1    2700 4100
	-1   0    0    1   
$EndComp
$Comp
L Device:C C41
U 1 1 60E75E0C
P 850 4050
F 0 "C41" H 965 4096 50  0000 L CNN
F 1 "470n" H 965 4005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 888 3900 50  0001 C CNN
F 3 "~" H 850 4050 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 850 4050 50  0001 C CNN "#manf"
	1    850  4050
	-1   0    0    1   
$EndComp
Wire Wire Line
	850  3900 1700 3900
Wire Wire Line
	1700 3900 1700 3950
Wire Wire Line
	1700 3900 1900 3900
Wire Wire Line
	1900 3900 1900 3950
Connection ~ 1700 3900
Wire Wire Line
	1900 3900 2700 3900
Wire Wire Line
	2700 3900 2700 3950
Connection ~ 1900 3900
Text HLabel 850  4200 0    50   Input ~ 0
GND
Text HLabel 2700 4250 0    50   Input ~ 0
GND
Text HLabel 1100 4350 0    50   Input ~ 0
SDA
Text HLabel 1100 4450 0    50   Input ~ 0
SCL
Text HLabel 1000 4950 0    50   Input ~ 0
GND
Text HLabel 1800 5350 3    50   Input ~ 0
GND
$Comp
L Device:C C43
U 1 1 60E84691
P 2750 5100
F 0 "C43" H 2865 5146 50  0000 L CNN
F 1 "470n" H 2865 5055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2788 4950 50  0001 C CNN
F 3 "~" H 2750 5100 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 2750 5100 50  0001 C CNN "#manf"
	1    2750 5100
	-1   0    0    1   
$EndComp
Wire Wire Line
	2750 4950 2500 4950
Text HLabel 2750 5250 3    50   Input ~ 0
GND
Text HLabel 3200 5250 3    50   Input ~ 0
GND
Text GLabel 2500 4350 2    50   Input ~ 0
MPU_INT
Text HLabel 1100 4550 0    50   Input ~ 0
GND
Text GLabel 7500 3050 0    50   Input ~ 0
MPU_INT
Wire Wire Line
	3200 4850 3200 4950
Wire Wire Line
	2500 4850 3200 4850
$Comp
L Device:C C51
U 1 1 60E88221
P 3200 5100
F 0 "C51" H 3315 5146 50  0000 L CNN
F 1 "470n" H 3315 5055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3238 4950 50  0001 C CNN
F 3 "~" H 3200 5100 50  0001 C CNN
F 4 "JMK105BJ474KV-F" H 3200 5100 50  0001 C CNN "#manf"
	1    3200 5100
	-1   0    0    1   
$EndComp
$Comp
L Regulator_Linear:HT75xx-1-SOT89 U12
U 1 1 60EC1755
P 10000 1700
F 0 "U12" H 10000 2067 50  0000 C CNN
F 1 "HT7533-1-SOT89" H 10000 1976 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-89-3" H 10000 2025 50  0001 C CIN
F 3 "https://www.holtek.com/documents/10179/116711/HT75xx-1v250.pdf" H 10000 1800 50  0001 C CNN
F 4 "C14289" H 10000 1700 50  0001 C CNN "LCSC"
	1    10000 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 60EC2191
P 9450 1750
AR Path="/5E2C3773/60EC2191" Ref="C?"  Part="1" 
AR Path="/5E080FD6/60EC2191" Ref="C1"  Part="1" 
F 0 "C1" H 9565 1796 50  0000 L CNN
F 1 "22u" H 9565 1705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9488 1600 50  0001 C CNN
F 3 "~" H 9450 1750 50  0001 C CNN
F 4 "GRM21BR61C226ME44L" H 9450 1750 50  0001 C CNN "#manf"
	1    9450 1750
	1    0    0    -1  
$EndComp
Text HLabel 9450 1600 0    50   Input ~ 0
BATT_VDD
Text HLabel 9450 1900 3    50   Input ~ 0
GND
Text HLabel 10000 1900 3    50   Input ~ 0
GND
$Comp
L Device:C C?
U 1 1 60EC5DE7
P 10450 1750
AR Path="/5E2C3773/60EC5DE7" Ref="C?"  Part="1" 
AR Path="/5E080FD6/60EC5DE7" Ref="C25"  Part="1" 
F 0 "C25" H 10565 1796 50  0000 L CNN
F 1 "22u" H 10565 1705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10488 1600 50  0001 C CNN
F 3 "~" H 10450 1750 50  0001 C CNN
F 4 "GRM21BR61C226ME44L" H 10450 1750 50  0001 C CNN "#manf"
	1    10450 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 1600 9700 1600
Wire Wire Line
	10300 1600 10450 1600
Text HLabel 10450 1900 3    50   Input ~ 0
GND
Wire Wire Line
	8300 700  8900 700 
Wire Wire Line
	10450 1600 10550 1600
Wire Wire Line
	10550 1600 10550 550 
Wire Wire Line
	10550 550  8400 550 
Wire Wire Line
	8400 550  8400 950 
Connection ~ 10450 1600
Text Label 10550 1600 0    50   ~ 0
3V3_ANALOG
Text GLabel 7500 3750 0    50   Output ~ 0
USB_TO_BUS_EN
Text GLabel 7500 3850 0    50   Output ~ 0
BATT_TO_BUS_EN
Text GLabel 4500 4350 0    50   Input ~ 0
USB_C_VDD
Text GLabel 8000 4600 2    50   Input ~ 0
USB_C_VDD
$Comp
L Connector_Generic:Conn_01x10 J1
U 1 1 60F597D4
P 4900 1150
F 0 "J1" H 4980 1142 50  0000 L CNN
F 1 "DEBUG" H 4980 1051 50  0000 L CNN
F 2 "Connector_FFC-FPC:Hirose_FH12-10S-0.5SH_1x10-1MP_P0.50mm_Horizontal" H 4900 1150 50  0001 C CNN
F 3 "~" H 4900 1150 50  0001 C CNN
	1    4900 1150
	1    0    0    -1  
$EndComp
Text HLabel 2950 7400 2    50   Input ~ 0
GND
Wire Wire Line
	3100 7300 2950 7300
Text Label 3100 7300 0    50   ~ 0
EXT
Text HLabel 2950 7200 2    50   Input ~ 0
INT
Text GLabel 2950 7100 2    50   Input ~ 0
MOTORS_TX
Text GLabel 2950 7000 2    50   Input ~ 0
MOTORS_RX
Text HLabel 2950 6900 2    50   Input ~ 0
BATT_VDD
Text HLabel 2950 6800 2    50   Input ~ 0
BATT_VDD
Text HLabel 2950 6700 2    50   Input ~ 0
GND
NoConn ~ 2950 6600
NoConn ~ 2950 6500
NoConn ~ 2950 6400
NoConn ~ 2950 6300
Text HLabel 2950 6200 2    50   Input ~ 0
GND
NoConn ~ 2950 6100
Text HLabel 2950 6000 2    50   Input ~ 0
GND
Text GLabel 2350 6300 0    50   Input ~ 0
SPI_MISO_MOSI
Text HLabel 2350 7400 0    50   Input ~ 0
GND
Wire Wire Line
	2350 7300 2150 7300
Text HLabel 2350 6900 0    50   Input ~ 0
BATT_VDD
Text HLabel 2350 6200 0    50   Input ~ 0
GND
$Comp
L Connector_Generic:Conn_01x15 J6
U 1 1 60FE83F3
P 2750 6700
F 0 "J6" H 2668 5775 50  0000 C CNN
F 1 "SHOE_B2" H 2668 5866 50  0000 C CNN
F 2 "rofi:SlipRing-120220-0161" H 2750 6700 50  0001 C CNN
F 3 "~" H 2750 6700 50  0001 C CNN
	1    2750 6700
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x15 J5
U 1 1 60FE5197
P 2550 6700
F 0 "J5" H 2500 7650 50  0000 L CNN
F 1 "SHOE_B1" H 2300 7550 50  0000 L CNN
F 2 "rofi:SlipRing-120220-0161" H 2550 6700 50  0001 C CNN
F 3 "~" H 2550 6700 50  0001 C CNN
	1    2550 6700
	1    0    0    -1  
$EndComp
Text Label 2150 7300 0    50   ~ 0
EXT
Text HLabel 2350 7200 0    50   Input ~ 0
INT
Text HLabel 2350 6000 0    50   Input ~ 0
GND
Text HLabel 2350 6700 0    50   Input ~ 0
GND
Text HLabel 2350 6800 0    50   Input ~ 0
BATT_VDD
Text GLabel 2350 6600 0    50   Input ~ 0
DOCK_6_CE
Text GLabel 2350 6500 0    50   Input ~ 0
DOCK_5_CE
Text GLabel 2350 6400 0    50   Input ~ 0
DOCK_4_CE
Text GLabel 2350 6100 0    50   Input ~ 0
SPI_SCK
Text GLabel 2350 7100 0    50   Input ~ 0
MOTORS_TX
Text GLabel 2350 7000 0    50   Input ~ 0
MOTORS_RX
Text HLabel 4400 6750 2    50   Input ~ 0
GND
Text HLabel 4400 6350 2    50   Input ~ 0
GND
Text HLabel 4400 6850 2    50   Input ~ 0
GND
Text HLabel 4400 6950 2    50   Input ~ 0
GND
Text HLabel 4400 7050 2    50   Input ~ 0
GND
Text HLabel 4400 6650 2    50   Input ~ 0
BATT_VDD
Text HLabel 4400 6550 2    50   Input ~ 0
BATT_VDD
Text HLabel 4400 6450 2    50   Input ~ 0
BATT_VDD
Text GLabel 4400 6250 2    50   Input ~ 0
MOTORS_RX
Text GLabel 4400 6150 2    50   Input ~ 0
MOTORS_TX
$Comp
L Connector_Generic:Conn_01x10 J4
U 1 1 61077600
P 4200 6550
F 0 "J4" H 4118 7167 50  0000 C CNN
F 1 "MOTORS" H 4118 7076 50  0000 C CNN
F 2 "Connector_FFC-FPC:Hirose_FH12-10S-0.5SH_1x10-1MP_P0.50mm_Horizontal" H 4200 6550 50  0001 C CNN
F 3 "~" H 4200 6550 50  0001 C CNN
	1    4200 6550
	-1   0    0    -1  
$EndComp
NoConn ~ 8800 3850
NoConn ~ 7500 2050
NoConn ~ 7500 2150
NoConn ~ 7500 2250
Text HLabel 1900 3750 1    50   Input ~ 0
3V3
Wire Wire Line
	1900 3750 1900 3900
Wire Wire Line
	5850 1150 7500 1150
Wire Wire Line
	6250 1350 6250 2150
Wire Wire Line
	6250 1350 7500 1350
$Comp
L Device:Crystal_GND24 Y1
U 1 1 61320ABB
P 6000 3000
F 0 "Y1" H 6194 3046 50  0000 L CNN
F 1 "7D024000H01" H 6200 3150 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_2016-4Pin_2.0x1.6mm" H 6000 3000 50  0001 C CNN
F 3 "~" H 6000 3000 50  0001 C CNN
F 4 "C648933" H 6000 3000 50  0001 C CNN "LCSC"
	1    6000 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C52
U 1 1 61322ABF
P 5650 3150
F 0 "C52" H 5765 3196 50  0000 L CNN
F 1 "10p" H 5765 3105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 5688 3000 50  0001 C CNN
F 3 "~" H 5650 3150 50  0001 C CNN
F 4 "C32949" H 5650 3150 50  0001 C CNN "LCSC"
	1    5650 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C65
U 1 1 613235E8
P 6400 3150
F 0 "C65" H 6515 3196 50  0000 L CNN
F 1 "10p" H 6515 3105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6438 3000 50  0001 C CNN
F 3 "~" H 6400 3150 50  0001 C CNN
F 4 "C32949" H 6400 3150 50  0001 C CNN "LCSC"
	1    6400 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 3000 5650 3000
Wire Wire Line
	6150 3000 6400 3000
Text HLabel 6400 3300 3    50   Input ~ 0
GND
Wire Wire Line
	5650 3300 6000 3300
Wire Wire Line
	6000 3200 6000 3300
Connection ~ 6000 3300
Wire Wire Line
	6000 3300 6400 3300
Text HLabel 6050 2750 2    50   Input ~ 0
GND
Wire Wire Line
	6050 2750 6000 2750
Wire Wire Line
	6000 2750 6000 2800
Wire Wire Line
	7000 3000 7000 1850
Wire Wire Line
	7000 1850 7500 1850
Wire Wire Line
	6400 3000 7000 3000
Connection ~ 6400 3000
Wire Wire Line
	7500 1750 6900 1750
Wire Wire Line
	6900 1750 6900 2650
Wire Wire Line
	6900 2650 5650 2650
Wire Wire Line
	5650 2650 5650 3000
Connection ~ 5650 3000
Wire Wire Line
	6000 2150 6250 2150
Wire Wire Line
	6450 1550 7500 1550
$EndSCHEMATC
