/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireControlSlave.

    FireControlSlave is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireControlSlave is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireControlSlave.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <HardwareSerial.h>



#define USE_BLUETOOTH
#define USE_RF24_SOFTSPI  // don't forget to uncomment SOFTSPI in RF24_config.h

/*#if defined(USE_RF24_SOFTSPI) && !defined(SOFTSPI)
#error You need to define SOFTSPI in RF24_config.h
error
#endif
*/

#ifdef USE_RF24_SOFTSPI
  #include <SoftSPI.h>
  #include <DigitalIO.h>
  #include <DigitalPin.h>
  #include <SoftI2cMaster.h>
  #include <I2cConstants.h>
  #include <PinIO.h>
#endif

#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>

//even after trimming, out of memory on UNO,
// must use a 2560 (mega) :-(
#include <printf.h>

/*
 * Radio nRF24
 * Alternate SPI Support

RF24 supports alternate SPI methods, in case the standard hardware SPI pins are otherwise unavailable.


Software Driven SPI

Software driven SPI is provided by the DigitalIO library

Setup:

    Install the digitalIO library
    Open RF24_config.h in a text editor. Uncomment the line #define SOFTSPI
    In your sketch, add #include DigitalIO.h

Note
    Note: Pins are listed as follows and can be modified by editing the RF24_config.h file

    //const uint8_t SOFT_SPI_MISO_PIN = 16;
    //const uint8_t SOFT_SPI_MOSI_PIN = 15;
    //const uint8_t SOFT_SPI_SCK_PIN = 14;
      const uint8_t SOFT_SPI_MISO_PIN = 22;  
      const uint8_t SOFT_SPI_MOSI_PIN = 23;
      const uint8_t SOFT_SPI_SCK_PIN = 24;    

 * 
 */


#include <LedAction.h>

#include <SPI.h>
#include <EEPROM.h>

#if defined(USE_BLUETOOTH)
  #include <RBL_nRF8001.h>
  #include <boards.h>
#endif

#include "SerialCommand.h"
#include "RadioComm.h"

#define FW_MSG_CMD_SET_SIGNAL_STR   7   // Set local signal strength

#define mega2560
//#define uno
//#define nano

#ifdef mega2560
  #define PIN_SPI_SS        53 //mega2560
  #define PIN_RF_CS         44 //8
  #define PIN_RF_CSN        45 //7
  #define PIN_RF_SCK        52
  #define PIN_RF_MOSI       51
  #define PIN_RF_MISO       50
  #define PIN_STATUS_LED    1
#endif

#ifdef uno
  #define PIN_STATUS_LED    5
  #define PIN_SPI_SS        53 //mega2560
  #define PIN_RF_CS         8
  #define PIN_RF_CSN        7
  #define PIN_RF_SCK        13
  #define PIN_RF_MOSI       11
  #define PIN_RF_MISO       12
#endif

#ifdef nano
  #define PIN_STATUS_LED    1
  #define PIN_SPI_SS        53 //mega2560
  #define PIN_RF_CS         3
  #define PIN_RF_CSN        4
  #define PIN_RF_SCK        19  //5
  #define PIN_RF_MOSI       17  //6
  #define PIN_RF_MISO       18  //7
#endif

SerialCommand   serialCommand(Serial);
RadioComm       radioComm(PIN_RF_CS, PIN_RF_CSN, 0xc2c2c2c2c2LL, 0xe7e7e7e7e7LL ); // reversed from firing slat


LedAction ledStatus;
void setup() 
{
  Serial.begin(115200);
  Serial.println("");
  // put your setup code here, to run once:

  pinMode(PIN_SPI_SS, OUTPUT); //set SPI slave select to output so mega is master https://www.arduino.cc/en/Reference/SPI
  digitalWrite(PIN_SPI_SS, HIGH);

  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, HIGH);
  
  serialCommand.init();

  LogDebug("Starting radio");
  radioComm.init();

  ledStatus.init(PIN_STATUS_LED, true);
  ledStatus.off();
  
  Serial.println("Done");
  
  // turn off onboard LED
  pinMode(13, OUTPUT);  
  digitalWrite(13, LOW);  

#if defined(USE_BLUETOOTH)
  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
  // Set your BLE Shield name here, max. length 10
  ble_set_name("KaBOOM");
  
  // Init. and start BLE library.
  ble_begin();
#endif  
}

void loop() 
{
  delay(1);

  ble_do_events();

  ledStatus.loop();
  
  uint8_t *pData = serialCommand.loop();
  if (pData)
  {
    uint16_t cmd = *((uint16_t *)(pData + 7));
    if (cmd == FW_MSG_CMD_SET_SIGNAL_STR)
    {
      uint8_t str = *((uint8_t *)(pData + 10));
      radioComm.setSignalStrength(str);
    }
    else
      radioComm.sendMessage(pData, RADIO_FRAME_SIZE);
  }
    
  pData= radioComm.loop();
  if (pData)
  {
//    ledStatus.fastBlinkTimed(1000, true);
ledStatus.fadeCycle(20000, true);

    //digitalWrite(PIN_STATUS_LED, LOW);
    serialCommand.sendData(pData, RADIO_FRAME_SIZE);
    
    delete pData;
  }
}
