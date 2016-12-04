# Wireless-Fireworks
Wireless fireworks controllers, slats, simulator

Project			Function
FireworksSimulator	Play the csv in a simulated environment. Hardcoded for a specific show.
FireworksGuiAndroid	Qt Android based controller
FireworksGui		Qt computer based controller (tested under Ubuntu 14)

FireModule		Code for slats
FireControlSlave	Send commands from computer or Android to slats.
                        To build Android version #define USE_BLUETOOTH
                        To build computer version undefine USE_BLUETOOTH
FireModuleTest		Run radio, display, and relay tests on a slat


FireControl		This was meant to be an Arduino based controller - never completed
FireComputer		Command line script player - used in original testing but never completed


Arduino Libraries
These libraries have github repos
	TMRh20 nRF24 library 				https://github.com/tmrh20/RF24/
	Docs Optimized High Speed NRF24L01+     	http://tmrh20.github.io/RF24

	nRF8001 BLueTooth 				https://github.com/RedBearLab/nRF8001


These libraries do not have github repos
ReadWriteEEPROM local library

ButtonHandler from From http://www.instructables.com/id/Arduino-Button-Tutorial/

TM1637 This was in a zip file from the eBay seller, but I also found it on github here https://github.com/linksprite/Linkerkit/tree/master/Linker_4D7SLED


*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

High level overview 

For Computer to Slat communications  (USB to NRF24 radio)

FireworksGui
    |
    |
   \|/
FireControlSlave (with USE_BLUETOOTH undefined)
    |
    |
   \|/
FireModule


For Andoird to Slat communications (BlueTooth to NRF24 radio)

FireworksGuiAndroid
    |
    |
   \|/
FireControlSlave (with USE_BLUETOOTH defined)
    |
    |
   \|/
FireModule


*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

Example show script is in
    FireworksGui/show2016.csv

Note: Most of the fields are hardcoded because I was in a rush to get this running for the Fourth of July holiday!
There is minimal error checking.



*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-


While this was a great project I've decided to buy a commercial system so there will likely not be any updates to this project.
This was a fun learning experience - until the deadline began to loom closer and closer then it became a mad dash to the finish line!
You'll notice the lack of comments - this is unfortunate, but as a side project aspects of a civilized project had to suffer.

GOOD LUCK and be SAFE!
