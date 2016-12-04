/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireModuleTest.

    FireModuleTest is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireModuleTest is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireModuleTest.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <HardwareSerial.h>
#include <printf.h>
//RF24 from TMRh20
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>

#include <SPI.h>


#include <EEPROM.h>

#include <ButtonHandler.h>
#include <TM1637.h>

#define USE_TM1637
#define USE_SEGMENT
#define USE_RELAY
#define USE_RADIO


#include "SegmentConfig.h"
#include "RelayModule.h"
#include "RadioComm.h"
#include "linkedlist.h"
#include "utils.h"


#define PIN_RELAY1        22
#define PIN_RELAY2        23
#define PIN_RELAY3        24
#define PIN_RELAY4        25
#define PIN_RELAY5        26
#define PIN_RELAY6        27
#define PIN_RELAY7        28
#define PIN_RELAY8        29
#define PIN_RELAY9        30
#define PIN_RELAY10       31
#define PIN_RELAY11       32
#define PIN_RELAY12       33
#define PIN_RELAY13       34
#define PIN_RELAY14       35
#define PIN_RELAY15       36
#define PIN_RELAY16       37
#define PIN_RELAY17       38
#define PIN_RELAY18       39


#define PIN_DIO           40
#define PIN_CLK           41
#define PIN_BUTTON_UP     42  // Button 1
#define PIN_BUTTON_DOWN   43  // Button 2

#define PIN_SPI_SS        53 //mega2560
#define PIN_RF_CS         44 //8
#define PIN_RF_CSN        45 //7

// maybe this chip
// http://playground.arduino.cc/Main/TM1637


#if defined(USE_RELAY) && defined(USE_RADIO)
  #define USE_PROCESS
#endif


#ifdef USE_TM1637
  TM1637 tm1637(PIN_CLK,PIN_DIO);
#endif

#ifdef USE_SEGMENT
  SegmentConfig segConfig(tm1637, PIN_BUTTON_UP, PIN_BUTTON_DOWN);
#endif

#ifdef USE_RELAY
  RelayModule relayModule;
#endif

#ifdef USE_RADIO
  RadioComm radioComm(PIN_RF_CS, PIN_RF_CSN, 0xe7e7e7e7e7LL, 0xc2c2c2c2c2LL );
#endif


#ifdef USE_RELAY
  int arrRelayPins[]={ PIN_RELAY1,   PIN_RELAY2,   PIN_RELAY3,   PIN_RELAY4, 
                    PIN_RELAY5,   PIN_RELAY6,   PIN_RELAY7,   PIN_RELAY8, 
                    PIN_RELAY9,   PIN_RELAY10,  PIN_RELAY11,  PIN_RELAY12,
                    PIN_RELAY13,  PIN_RELAY14,  PIN_RELAY15,  PIN_RELAY16,
                    PIN_RELAY17,  PIN_RELAY18 };

  int numRelayPins = sizeof(arrRelayPins)/sizeof(int);
#endif  


int8_t arrDisp[4] = {0x00, 0x01, 0x02, 0b00000111};
void setup() 
{
  Serial.begin(115200);
  pinMode(PIN_SPI_SS, OUTPUT); //set SPI slave select to output so mega is master https://www.arduino.cc/en/Reference/SPI

  uint8_t u8Control = 0;
  uint8_t u8Slat    = 0;
  
#ifdef USE_RELAY
  relayModule.init(arrRelayPins, numRelayPins, false);
#endif  

  uint8_t arr[4];

  Serial.println("\n\nStarting");
  // put your setup code here, to run once:
#ifdef USE_TM1637  
  LogDebug("Starting TM1637");
  tm1637.set();
  tm1637.init();
  tm1637.clearDisplay();
  LogDebug("TM1637 started");
#endif

#ifdef USE_SEGMENT
  LogDebug("Starting 7Segment");
  segConfig.init();
  u8Control = segConfig.controlId();
  u8Slat    = segConfig.slatId();
  LogDebug("7Segment started");
  arr[0] = 0; arr[1]= 0; arr[2]= 0; arr[3]= 1;
  segConfig.printUser(arr,1000);
#endif

#ifdef USE_RADIO
  LogDebug("Starting radio");
  radioComm.init();
  LogDebug("Radio started");
  arr[0] = 0; arr[1]= 0; arr[2]= 0; arr[3]= 2;
  segConfig.printUser(arr,1000);
  delay(500);
#endif

  Serial.println("Done");
  
  // turn off onboard LED
  pinMode(13, OUTPUT);  
  digitalWrite(13, LOW);  

  arr[0] = 0; arr[1]= 0; arr[2]= 0; arr[3]= 4;
  segConfig.printUser(arr,1000);
  delay(250);

}

int iDebug = 0;
int iStillAlive = 0;
unsigned long tLast = 0;
void loop() 
{
  delay(1);


#ifdef USE_SEGMENT
  segConfig.loop();
#endif

#ifdef USE_RELAY
  relayModule.loop();
#endif

#ifdef USE_RADIO
  uint8_t *pData = radioComm.loop();
#endif

#ifdef USE_RELAY
  if ((millis() - tLast) > 5000)
  {
    for (int x=1; x < numRelayPins+1; x++)
    {
      relayModule.relayOn(x-1);
      uint8_t arr[4] = {LET_DASH, 0, x/10, x - ((x/10) * 10)};
      #ifdef USE_RADIO
        if (radioComm.valid())
          arr[0] = LET_A;
      #endif
      segConfig.printUser(arr,250);
      delay(250);
    }

   tLast = millis();
  }
#endif

}
