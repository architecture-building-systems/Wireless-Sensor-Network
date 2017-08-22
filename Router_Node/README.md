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

