EESchema Schematic File Version 2  date Tue 24 Jul 2012 08:25:57 PM COT
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:usba-plug
LIBS:usb_a
LIBS:fsusb20
LIBS:usbconn
LIBS:con-cypressindustries
LIBS:at45db321d
LIBS:devices_mod
LIBS:smd-special
LIBS:my_dev
LIBS:ft2232d
LIBS:xilinx_virtexii-xc2v80&flashprom
LIBS:m25pxx
LIBS:lacedaemonia-cache
EELAYER 25  0
EELAYER END
$Descr A 11000 8500
encoding utf-8
Sheet 4 7
Title "CondorBoard_V1"
Date "25 jul 2012"
Rev "V1"
Comp "Uniandes"
Comment1 "Josnelihurt Rodriguez Barajas"
Comment2 ""
Comment3 "Autores:"
Comment4 ""
$EndDescr
Wire Wire Line
	4800 2450 4800 2550
Connection ~ 4800 1050
Wire Wire Line
	4800 1050 3550 1050
Connection ~ 2250 1050
Wire Wire Line
	6750 1300 6850 1300
Connection ~ 6600 1400
Wire Wire Line
	6600 1400 6650 1400
Wire Wire Line
	6650 1400 6650 1500
Wire Wire Line
	6650 1500 6950 1500
Wire Wire Line
	6950 1500 6950 1250
Connection ~ 7350 1250
Wire Wire Line
	7400 1250 7300 1250
Wire Wire Line
	7200 1250 7250 1250
Connection ~ 6600 1300
Wire Wire Line
	6650 1300 6550 1300
Wire Wire Line
	6450 1300 6100 1300
Wire Wire Line
	3450 1050 3450 1150
Wire Wire Line
	3450 1150 3400 1150
Wire Wire Line
	7100 1100 7100 1400
Wire Wire Line
	7450 1350 7450 1250
Wire Wire Line
	7850 2050 7850 1800
Wire Wire Line
	7350 1250 7350 1400
Wire Wire Line
	7350 1400 7850 1400
Wire Wire Line
	7850 1400 7850 1150
Wire Wire Line
	7850 1150 7900 1150
Wire Wire Line
	7250 1250 7250 1400
Wire Wire Line
	7250 1400 7100 1400
Connection ~ 2600 1050
Wire Wire Line
	2600 1050 2600 1500
Wire Wire Line
	6100 1300 6100 1100
Connection ~ 2050 2200
Wire Notes Line
	850  5050 850  5400
Wire Wire Line
	4100 1650 4100 1900
Connection ~ 2450 1050
Connection ~ 1700 1050
Wire Wire Line
	4800 1450 4800 850 
Wire Wire Line
	4800 1950 4800 1850
Wire Wire Line
	2050 2200 2050 2450
Wire Wire Line
	3550 1050 3550 1300
Wire Wire Line
	3550 1300 3300 1300
Wire Wire Line
	3300 1300 3300 1050
Wire Wire Line
	2600 1900 2600 2200
Wire Wire Line
	2600 2200 1700 2200
Connection ~ 2250 2200
Wire Wire Line
	3400 1150 3400 1200
Wire Wire Line
	3250 1050 3350 1050
Connection ~ 3300 1050
Wire Wire Line
	7450 1250 7500 1250
Wire Wire Line
	6600 1300 6600 1450
Wire Wire Line
	6850 1300 6850 1400
Connection ~ 6750 1300
Wire Wire Line
	950  900  950  1050
Wire Wire Line
	950  1050 3150 1050
Wire Wire Line
	2250 800  2250 1050
Wire Wire Line
	4100 1250 4100 1050
Connection ~ 4100 1050
$Comp
L GND #PWR?
U 1 1 500F4B14
P 4800 2550
F 0 "#PWR?" H 4800 2550 30  0001 C CNN
F 1 "GND" H 4800 2480 30  0001 C CNN
	1    4800 2550
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR031
U 1 1 4FCD3AAA
P 950 900
F 0 "#PWR031" H 950 1000 30  0001 C CNN
F 1 "VCC" H 950 1000 30  0000 C CNN
	1    950  900 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR032
U 1 1 4FAF11F4
P 6600 1850
F 0 "#PWR032" H 6600 1850 30  0001 C CNN
F 1 "GND" H 6600 1780 30  0001 C CNN
	1    6600 1850
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR033
U 1 1 4FAF11F3
P 7850 2050
F 0 "#PWR033" H 7850 2050 30  0001 C CNN
F 1 "GND" H 7850 1980 30  0001 C CNN
	1    7850 2050
	-1   0    0    -1  
$EndComp
$Comp
L +5V #PWR034
U 1 1 4EAB61F9
P 7100 1100
F 0 "#PWR034" H 7100 1190 20  0001 C CNN
F 1 "+5V" H 7100 1190 30  0000 C CNN
	1    7100 1100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR035
U 1 1 4EAB60AE
P 7450 1350
F 0 "#PWR035" H 7450 1350 30  0001 C CNN
F 1 "GND" H 7450 1280 30  0001 C CNN
	1    7450 1350
	-1   0    0    -1  
$EndComp
$Comp
L C C1604
U 1 1 4E00F175
P 7850 1600
F 0 "C1604" H 7900 1700 50  0000 L CNN
F 1 "10uF" H 7900 1500 50  0000 L CNN
	1    7850 1600
	1    0    0    -1  
$EndComp
$Comp
L +1.2V #PWR036
U 1 1 4E00DE2E
P 7900 1150
F 0 "#PWR036" H 7900 1290 20  0001 C CNN
F 1 "+1.2V" H 7900 1260 30  0000 C CNN
	1    7900 1150
	1    0    0    -1  
$EndComp
$Comp
L AP1122 U1601
U 1 1 4E00DD41
P 7350 1000
F 0 "U1601" H 7450 1350 60  0000 C CNN
F 1 "AP1122_1.2" H 7250 1250 60  0000 C CNN
F 4 "http://search.digikey.com/scripts/DkSearch/dksus.dll?Detail&name=AP1122EGDITR-ND" H 7350 1000 60  0001 C CNN "Buy"
	1    7350 1000
	-1   0    0    -1  
$EndComp
$Comp
L +5V #PWR037
U 1 1 4EAB61BB
P 6100 1100
F 0 "#PWR037" H 6100 1190 20  0001 C CNN
F 1 "+5V" H 6100 1190 30  0000 C CNN
	1    6100 1100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 4EAB60B3
P 6850 1400
F 0 "#PWR038" H 6850 1400 30  0001 C CNN
F 1 "GND" H 6850 1330 30  0001 C CNN
	1    6850 1400
	-1   0    0    -1  
$EndComp
$Comp
L AP1122 U1402
U 1 1 4EAB5F97
P 6600 1050
F 0 "U1402" H 6750 1300 60  0000 C CNN
F 1 "AP1122_2.5" H 6650 1400 60  0000 C CNN
F 4 "http://search.digikey.com/scripts/DkSearch/dksus.dll?Detail&name=AP1122EGDITR-ND" H 6600 1050 60  0001 C CNN "Buy"
	1    6600 1050
	-1   0    0    -1  
$EndComp
$Comp
L AP1122 U1401
U 1 1 4EAB5F92
P 3300 800
F 0 "U1401" H 3450 1050 60  0000 C CNN
F 1 "AP1122_3.3" H 3250 1150 60  0000 C CNN
F 4 "http://search.digikey.com/scripts/DkSearch/dksus.dll?Detail&name=AP1122EGDITR-ND" H 3300 800 60  0001 C CNN "Buy"
	1    3300 800 
	-1   0    0    -1  
$EndComp
$Comp
L R R1401
U 1 1 4EAB5DF2
P 4800 2200
F 0 "R1401" V 4880 2200 50  0000 C CNN
F 1 "330R" V 4800 2200 50  0000 C CNN
	1    4800 2200
	1    0    0    -1  
$EndComp
$Comp
L LED D1401
U 1 1 4EAB5DEC
P 4800 1650
F 0 "D1401" H 4800 1750 50  0000 C CNN
F 1 "LED" H 4800 1550 50  0000 C CNN
	1    4800 1650
	0    -1   1    0   
$EndComp
$Comp
L +5V #PWR039
U 1 1 4E00C0FF
P 2250 800
F 0 "#PWR039" H 2250 890 20  0001 C CNN
F 1 "+5V" H 2250 890 30  0000 C CNN
	1    2250 800 
	1    0    0    -1  
$EndComp
Text Label 1250 1050 0    60   ~ 0
PRE_5
$Comp
L C C1605
U 1 1 4E00ED88
P 6600 1650
F 0 "C1605" H 6650 1750 50  0000 L CNN
F 1 "10uF" H 6650 1550 50  0000 L CNN
	1    6600 1650
	1    0    0    -1  
$EndComp
$Comp
L C C1603
U 1 1 4E00ED7A
P 2600 1700
F 0 "C1603" H 2650 1800 50  0000 L CNN
F 1 "47uF" H 2650 1600 50  0000 L CNN
	1    2600 1700
	1    0    0    -1  
$EndComp
$Comp
L +2.5V #PWR040
U 1 1 4E00ED55
P 6950 1250
F 0 "#PWR040" H 6950 1200 20  0001 C CNN
F 1 "+2.5V" H 6950 1350 30  0000 C CNN
	1    6950 1250
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR041
U 1 1 4DF92B34
P 4800 850
F 0 "#PWR041" H 4800 810 30  0001 C CNN
F 1 "+3.3V" H 4800 960 30  0000 C CNN
	1    4800 850 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR042
U 1 1 4DF92B07
P 4100 1900
F 0 "#PWR042" H 4100 1900 30  0001 C CNN
F 1 "GND" H 4100 1830 30  0001 C CNN
	1    4100 1900
	1    0    0    -1  
$EndComp
$Comp
L C C1602
U 1 1 4DF92AF1
P 4100 1450
F 0 "C1602" H 4150 1250 50  0000 L CNN
F 1 "10uF" H 4150 1350 50  0000 L CNN
	1    4100 1450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR043
U 1 1 4DF929B1
P 2050 2450
F 0 "#PWR043" H 2050 2450 30  0001 C CNN
F 1 "GND" H 2050 2380 30  0001 C CNN
	1    2050 2450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR044
U 1 1 4DF9299E
P 3400 1200
F 0 "#PWR044" H 3400 1200 30  0001 C CNN
F 1 "GND" H 3400 1130 30  0001 C CNN
	1    3400 1200
	1    0    0    -1  
$EndComp
Text HLabel 1200 1650 0    60   Input ~ 0
PWR_LED
$EndSCHEMATC