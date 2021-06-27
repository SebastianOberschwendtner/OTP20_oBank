EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 5
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
L MCU_ST_STM32L0:STM32L071KZTx U?
U 1 1 60D7752C
P 6060 3020
F 0 "U?" H 6010 1931 50  0000 C CNN
F 1 "STM32L071KZTx" H 6010 1840 50  0000 C CNN
F 2 "Package_QFP:LQFP-32_7x7mm_P0.8mm" H 5560 2120 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00141136.pdf" H 6060 3020 50  0001 C CNN
	1    6060 3020
	1    0    0    -1  
$EndComp
Text HLabel 2050 2100 0    50   Input ~ 0
3.3V
$Comp
L power:+3V3 #PWR0101
U 1 1 60D7AFB2
P 2150 1900
F 0 "#PWR0101" H 2150 1750 50  0001 C CNN
F 1 "+3V3" H 2165 2073 50  0000 C CNN
F 2 "" H 2150 1900 50  0001 C CNN
F 3 "" H 2150 1900 50  0001 C CNN
	1    2150 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 2100 2150 1900
Wire Wire Line
	2150 2100 2050 2100
$EndSCHEMATC
