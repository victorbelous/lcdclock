/* 
	Editor: https://www.visualmicro.com/
			visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			the contents of the _vm sub folder can be deleted prior to publishing a project
			all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Uno, Platform=avr, Package=arduino
*/

#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define _VMDEBUG 1
#define ARDUINO 108012
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define F_CPU 16000000L
#define ARDUINO 108012
#define ARDUINO_AVR_UNO
#define ARDUINO_ARCH_AVR
byte pressButton(byte Pin);
void printlcd(String lcdLine1, String lcdLine2);
byte lcdprint (byte displ, String linepr1, String linepr2);
void editDoW();
void editDay();
void editMonth();
void editYear();
void editMin();
void editHour();
void resetSec();
void displayCheckAlarm ();
void editAlarm ();
//
//

#include "pins_arduino.h" 
#include "arduino.h"
#include "liquidrus2.ino"