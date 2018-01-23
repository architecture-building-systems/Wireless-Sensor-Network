# Sensor Node
![Sensor Node](Images/DSC02624.JPG) 
Sensor node integrate sensors, power management and wireless communication. The sensor nodes itself is modular and consists of one communication board (CB) and one sensor board (SB). The two PCBs and the battery are protected by a 3D printed housing, consisting of two parts that latch together.
The two components can be exchanged independently and are connected by power bus and a serial communication bus. 
The sensor board has a microcontroller, which interfaces the onboard sensors. The sensor data is transferred in a standardized format via the serial communication bus to the communication board. The communication board receives the readings form the sensor board, saves them with a timestamp from the real time clock (RTC) to the SD-card and sends the data via ZigBee to the gateway.

## XBee Settings for Sensor Nodes (/Communication Boards)
In the current setting we are using XBee S2c (XB24CZ7WIT-004). We use the Firmware for the product family XB24C with the Function set ZIGBEE TH Reg. The Firmware version is 405E.
We use the default settings. Excpetions are listed below:

|Parameter | Value	| Comment |
|----------|--------|-------|
|ID	       | xxx	  | PAN ID, can be set to anything, but all nodes in the same network must have the same PAN ID |
|AP	       | 2	    | API mode with escaping |
|SM        | 1      | Allows sleep mode to be controlled through sleep pin|

The configuration profiles are also available as [files](XBee_Profiles/). All XBees corresponding to one coordinator need to be set to the same PAN ID as the coordinator.
