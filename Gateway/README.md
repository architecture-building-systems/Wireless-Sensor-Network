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

## XBee Settings for Gateway
In the current setting we are using XBee S2c (XB24CZ7WIT-004). We use the Firmware for the product family XB24C with the Function set ZIGBEE TH Reg. The Firmware version is 405E.
We use the default settings. Excpetions are listed below:

|Parameter | Value	| Comment |
|----------|---------|-------|
|ID	       | xxx	    | PAN ID, can be set to anything, but all nodes in the same network must have the same PAN ID |
|CE	       | 1	      | Set if this device is a coordinator (1) or not (0) |
|AP	       | 2	      | API mode with escaping |
|SP	       | AF0     | SP and SN together regulate after how long an end device is forgotten. We set it to the maximum time so that when waking up, our sensor modules are still in the network and not forgotten.|
|SN	       |FFFF     |  |	

The configuration profiles are also available as [files](XBee_Profiles/). All XBees corresponding to one coordinator need to be set to the same PAN ID as the coordinator.
