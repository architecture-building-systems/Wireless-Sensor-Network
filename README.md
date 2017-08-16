# Wireless Sensor Network

## Introduction
This repository contains all files needed to reproduce the Wireless Sensor Network (WSN) used for the Data-Driven Building Retrofit Research of the [A/S Chair](http://www.systems.arch.ethz.ch/). 
The WSN is intended to be deployed in residential buildings to collect sensor data for building model calibration. The microcontroller boards are based on the Arduino framework. For wireless communication, XBee radio modules are used.

## Structure of the WSN
The WSN has a modular structure. The main components are the following:
* Sensor Node
* Gateway
* Router Node

On the sensor node, the sensor readings are performed and the data is relayed over router nodes to the gateway using the XBee network. The gateway forwards the data over 3G cellular network to a web server, where the data is stored in a MySQL database. It is also possible to use a sensor node as data logger without any radio communication.

### Sensor Node
Sensor nodes consist of two parts: the communication board and the sensor board. The sensor board interfaces the actual sensor(s). It reads out the sensor reading and transmits the result via the serial bus in a unified format to the communication board. The communication board interfaces the sensor board, radio module, and the battery. It forwards the readings from the sensor board to the gateway. Both, the communication board and the sensor board, can be programmed using the Arduino framework.

#### Communication Board
The communication board is the same on all sensor nodes. It entails the following:
* Microcontroller (ATMega328p)
* Real-time clock (DS3231)
* Battery (2000mAh LiPo)
* SD-card socket
* XBee radio module (XB24CZ7WIT-004)
* Slide switch and LEDs for debugging
* Power jack for external power
* Connectors to sensor board

The tasks of the communication board are:
* Scheduling (RTC or interrupt from sensor board)
* Interfacing the sensor board
* Interfacing Xbee module
* Interfacing SD-card
* Reading the charge state of the battery
* Switching between normal operation and debugging mode

#### Sensor Board
The sensor board interfaces the actual sensor(s). Each sensor or sensor configuration needs its own sensor board design.
The following sensor boards are available:
* Temperature and Humidity (SHT31)
* Luminosity (TSL2561)
* Pulse
* CO2 (S8)
* Heat Flux (gSKIN XO)
* Dual Temperature (DS18B20)
* Prototyping

There are two kinds of sensor boards: sensor boards for periodic measurements (fixed time interval) and sensor boards for event based measurements (interrupt driven). On the first power-up of the communication board, the type of the sensor board is requested, in order to set the mode to periodic measurements or event based measurements. For periodic measurements, the communication board is woken up by the RTC on the communication board. It then turns on the power to the sensor board. After power-up of the sensor board, it reads out the sensor(s) and transmits the readings over the serial bus. Then the power to the sensor board is cut by the communication board and the data is written to the SD-card and/or sent to the gateway via radio.
For event based measurements the sensor board is always powered on. The communication board is woken up by an interrupt signal from the sensor board instead of the RTC. The data transfer is the same as for the periodic measurements. 

### Gateway
The gateway receives the data from the sensor nodes and relays the data to a web server via 3G cellular network. The gateway consists of Adafruit Feather 32u4 FONA and a XBee radio-module. The Adafruit Feather 32u4 FONA can be programmed using the Arduino framework. The gateway is powered via USB power supply and needs to be always powered on.

### [Router Node](Router_Node/)
A router node consists of XBee-radio module and SparkFun XBee Explorer Regulated Board. The router node is powered by a wall adapter power supply. It always needs to be powered on.
