/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireModule.

    FireModule is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireModule is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireModule.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <HardwareSerial.h>
#include <printf.h>
//RF24 from TMRh20
#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_UNO)
  #include <nRF24L01.h>
#endif
#include <RF24_config.h>
#include <RF24.h>

#include <SPI.h>


#include <EEPROM.h>
#include <ReadWriteEEPROM.h>

#include <ButtonHandler.h>

#include <TM1637.h>


#include "Config.h"
#include "RelayModule.h"
#include "RadioComm.h"
#include "linkedlist.h"
#include "utils.h"
#include "ProcessMessage.h"


#if defined(ARDUINO_AVR_MINI) || defined(ARDUINO_AVR_DUEMILANOVE)

  #define PIN_LED13         13
  
  #define PIN_RELAY1        22
  #define PIN_RELAY2        23
  #define PIN_RELAY3        24
  #define PIN_RELAY4        25
  #define PIN_RELAY5        26
  #define PIN_RELAY6        27

  #define PIN_DIO           A2
  #define PIN_CLK           A1
  #define PIN_BUTTON_UP     A0  // Button 1
  #define PIN_BUTTON_DOWN   99  // Button 1

  #define PIN_SPI_SS        10 //mega2560
  #define PIN_RF_CS         44 //8
  #define PIN_RF_CSN        45 //7
  #define PIN_SPI_SCK       13
  #define PIN_SPI_MOSI      11
  #define PIN_SPI_MISO      12
  
  #define PIN_RELAY_RESET    57  // control relay that will reset arduino
  #define PIN_RESET          56  // actual pin to control reset, tried direct from output pin to RST - not a good idea, got caught in a reset loop

  
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_UNO)

  #define PIN_LED13         13
  
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
  #define PIN_SPI_SCK       52
  #define PIN_SPI_MOSI      51
  #define PIN_SPI_MISO      50
  
  #define PIN_RELAY_RESET    7  // control relay that will reset arduino
  #define PIN_RESET          6  // actual pin to control reset, tried direct from output pin to RST - not a good idea, got caught in a reset loop
#endif

// maybe this chip
// http://playground.arduino.cc/Main/TM1637


int             g_iResetCountdown = 0;

TM1637 *         tm1637;
RelayModule*     relayModule;
RadioComm*       radioComm;
Segment*         segment;
Config*          cfg;
ProcessMessage*  processMessage;

void setup() 
{
  Serial.begin(115200);

  tm1637          = new TM1637(PIN_CLK,PIN_DIO);
  relayModule     = new RelayModule;;
  radioComm       = new RadioComm(PIN_RF_CS, PIN_RF_CSN, 0xe7e7e7e7e7LL, 0xc2c2c2c2c2LL );
  segment         = new Segment(*tm1637);
  cfg             = new Config(*segment, *radioComm, PIN_BUTTON_UP, PIN_BUTTON_DOWN);
  processMessage  = new ProcessMessage(*radioComm, *relayModule, *segment, *cfg);
  
  pinMode(PIN_SPI_SS, OUTPUT); //set SPI slave select to output so mega is master https://www.arduino.cc/en/Reference/SPI

  pinMode(PIN_LED13, OUTPUT);  // turn off LED on pin 13
  
  pinMode(PIN_RELAY_RESET, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  digitalWrite(PIN_RELAY_RESET, HIGH); //relay off
  digitalWrite(PIN_RESET, LOW);
  
  uint8_t u8Control = 0;
  uint8_t u8Slat    = 0;
  
#if defined(ARDUINO_AVR_MINI) || defined(ARDUINO_AVR_DUEMILANOVE)
  int arrRelayPins[]={ PIN_RELAY1,   PIN_RELAY2,   PIN_RELAY3,   PIN_RELAY4, 
                    PIN_RELAY5,   PIN_RELAY6 };
#else
  int arrRelayPins[]={ PIN_RELAY1,   PIN_RELAY2,   PIN_RELAY3,   PIN_RELAY4, 
                    PIN_RELAY5,   PIN_RELAY6,   PIN_RELAY7,   PIN_RELAY8, 
                    PIN_RELAY9,   PIN_RELAY10,  PIN_RELAY11,  PIN_RELAY12,
                    PIN_RELAY13,  PIN_RELAY14,  PIN_RELAY15,  PIN_RELAY16,
                    PIN_RELAY17,  PIN_RELAY18 };
#endif
  int numRelayPins = sizeof(arrRelayPins)/sizeof(int);

  relayModule->init(processMessage, arrRelayPins, numRelayPins, false);

  uint8_t arr[4];

  Serial.println("\n\nStarting");
  
  // put your setup code here, to run once:
  LogDebug("Starting TM1637");
  tm1637->set();
  tm1637->init();
  tm1637->clearDisplay();
  LogDebug("TM1637 started");

  LogDebug("Starting 7Segment");
  segment->init();
  LogDebug("7Segment started");
  arr[0] = 0; arr[1]= 0; arr[2]= 0; arr[3]= 1;
  segment->print(arr,1000);

  cfg->init();
  u8Control = cfg->controlId();
  u8Slat    = cfg->slatId();

  LogDebug("Starting radio");
  radioComm->init(cfg->radioPower());
  LogDebug("Radio started");
  arr[0] = 0; arr[1]= 0; arr[2]= 0; arr[3]= 2;
  segment->print(arr,1000);
  delay(500);

  LogDebug("Starting message processor");
  processMessage->init(0xdead);
  LogDebug("Message processor started");
  arr[0] = 0; arr[1]= 0; arr[2]= 0; arr[3]= 3;
  segment->print(arr,1000);

  Serial.println("Done");
  
  // turn off onboard LED
  pinMode(13, OUTPUT);  
  digitalWrite(13, LOW);  

  arr[0] = 0; arr[1]= 0; arr[2]= 0; arr[3]= 4;
  segment->print(arr,1000);
  delay(250);

}

void loop() 
{
  delay(1);

  segment->loop();
  cfg->loop();

  relayModule->loop();
  uint8_t *pData = radioComm->loop();

  // check if board should be reset
  if (processMessage->loop(pData, RADIO_FRAME_SIZE) == 0xdead)
  {
    segment->printstr("b4E", 0);
    delay(1000);
    g_iResetCountdown = 250;
  }

  if (g_iResetCountdown)
  {
    if (g_iResetCountdown == 1)
    {
      digitalWrite(PIN_RELAY_RESET,LOW);
      digitalWrite(PIN_RESET, LOW);  
    }
    else
      g_iResetCountdown--;
  }
  
}
