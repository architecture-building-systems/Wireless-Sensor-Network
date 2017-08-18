# Gateway 
![Gateway](Images/Gateway_collage.jpg) 
A gateway includes the following parts: 
* [Adafruit Feather 32u4 FONA](https://learn.adafruit.com/adafruit-feather-32u4-fona)
* Cellular Antenna
* uFL to SMA Adapter cable
* LiPo Battery (1000mAh) 
* XBee Module 
* USB Power Supply
* Custom PCB 
* 3D Printed Housing 

The router nodes receives the messages form the sensor node with the XBee radio module and forwards the messages over 3G to the webserver. This is done by reading the Xbee payload message and calling a PHP script running on the webserver. The XBee payload is sent as an GET parameter in the URL of the PHP script.
The XBee radio module and the Adafruit Feather 32u4 FONA are both plugged in to the custom PCB. The PCB features pin headers for the Feather 32u4 FONA and XBee radio module. Additionally, there are three indicator LEDs on the PCB. 

