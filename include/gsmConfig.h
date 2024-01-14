/*********************************************************************************************************************************
  defines.h

  For ESP8266, ESP32, SAMD21/SAMD51, nRF52, SAM DUE, Teensy and STM32 with GSM modules

  GSM_Generic is a library for the ESP8266, ESP32, SAMD21/SAMD51, nRF52, SAM DUE, Teensy and STM32 with GSM modules

  Based on and modified from MKRGSM Library (https://github.com/arduino-libraries/MKRGSM)

  Built by Khoi Hoang https://github.com/khoih-prog/GSM_Generic
  Licensed under GNU Lesser General Public License

  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, either version 2.1 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************************************************************/

#ifndef defines_h
#define defines_h

#define DEBUG_GSM_GENERIC_PORT Serial

// Debug Level from 0 to 5. Level 5 is to print out AT commands and responses
#define _GSM_GENERIC_LOGLEVEL_ 0

#define SECRET_PINNUMBER ""

#define GSM_USING_SOFTWARE_SERIAL true
#define SerialGSM gsmSerial
#define GSM_MODEM_SIM900 true

// libraries
#include "GSM_Generic_Main.h"

#endif // defines_h
