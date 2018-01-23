# Router Node
![Router Node](Images/Router_node_with_open_housing_1.jpg)
Router nodes relay messages from the sensor nodes to the gateway. A router node is basically a XBee radio module power by a wall adapter power supply. No additional microcontroller is needed.

A router node consists of the following parts:
* [SparkFun XBee Explorer Regulated](https://www.sparkfun.com/products/11373)
* XBee Module
* USB Power Supply
* 3D Printed Housing

The USB jack was removed from the power supply and the remaining cables were soldered to the XBee Explorer board. The XBee Explorer board can be stuck into the four stand offs on the bottom half of the 3D printed housing. A zip tie was put around the power supply cable and placed inside the housing as a strain-relief. The top and bottom part of the housing can be pushed together and are held together by friction. The CAD files can be found [here](CAD/).
The profiles to program the XBee modules of the router nodes can be [here](Xbee_Profiles/). The files are named according to the following scheme: Router_R[Sensor Kit Number]8[Router Number].

## XBee Settings for Router Nodes
In the current setting we are using XBee S2c (XB24CZ7WIT-004). We use the Firmware for the product family XB24C with the Function set ZIGBEE TH Reg. The Firmware version is 405E.
We use the default settings. Excpetions are listed below:

|Parameter| Value	| Comment |
|----------|---------|-------|
|ID	       | xxx	    | PAN ID, can be set to anything, but all nodes in the same network must have the same PAN ID |
|CE	       | 0	      | Set if this device is a coordinator (1) or not (0) |
|AP	       | 2	      | API mode with escaping |
|SP	       | AF0     | SP and SN together regulate after how long an end device is forgotten. We set it to the maximum time so that when waking up, our sensor modules are still in the network and not forgotten.|
|SN	       |FFFF     |  |	

The configuration profiles are also available as [files](/Xbee_Profiles). The only setting that needs to be changed from the corresponding coordinator is CE. All XBees corresponding to one coordinator need to be set to the same PAN ID as the coordinator.

