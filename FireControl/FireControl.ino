/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireControl.

    FireControl is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireControl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireControl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <HardwareSerial.h>

//even after trimming, out of memory on UNO,
// must use a 2560 (mega) :-(
#include <printf.h>

#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>

#include <SPI.h>


#include <EEPROM.h>

#include "ManagerSlat.h"
#include "SerialCommand.h"
#include "ProcessMessage.h"
#include "RadioComm.h"

#define PIN_SPI_SS        53 //mega2560
#define PIN_RF_CS         44 //8
#define PIN_RF_CSN        45 //7

// sounded like a good idea, but mem problems with UNO
// trying different, but not as clean
//linkedlist<uint8_t *> msgIncoming;
//linkedlist<uint8_t *> msgOutgoing;

ManagerSlat     managerSlat;
SerialCommand   serialCommand(Serial, managerSlat);
RadioComm       radioComm(PIN_RF_CS, PIN_RF_CSN, 0xc2c2c2c2c2LL, 0xe7e7e7e7e7LL ); // reversed from firing slat
ProcessMessage  processMessage(radioComm, managerSlat, serialCommand);

void testing()
{
  managerSlat.addSlat(1,16);
  managerSlat.addSlat(16,16);
}
void setup() 
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting");
  // put your setup code here, to run once:

  pinMode(PIN_SPI_SS, OUTPUT); //set SPI slave select to output so mega is master https://www.arduino.cc/en/Reference/SPI

  managerSlat.setPin(0xdead);
  managerSlat.setControlId(0xff);
  
  LogDebug("Starting serial");
  serialCommand.init();
  LogDebug("Serial started");

  LogDebug("Starting radio");
  radioComm.init();
  LogDebug("Radio started");

  LogDebug("Starting message processor");
  processMessage.init();
  LogDebug("Message processor started");

  Serial.println("Done");
  
  // turn off onboard LED
  pinMode(13, OUTPUT);  
  digitalWrite(13, LOW);  

  testing();
}

void loop() 
{
  delay(1);

  serialCommand.loop();

  uint8_t*pData= radioComm.loop();
  processMessage.loop(pData, RADIO_FRAME_SIZE);
  

}
