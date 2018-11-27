#################################################################
# Python serial sensor module reader                            #
#                                                               #
# This scripts reads the serial output form a sensor module and #
# translates the output to human readable form                  #
# Author: Mario Frei mario.frei@arch.ethz.ch                    #
#                                                               #
# Date: 10.11.2016                                              #
#################################################################


import time
import serial
import struct

SerialPort = 'COM28'

print('Python serial sensor module reader')
print('')

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port=SerialPort,
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()

try:
    while 1:
        if ser.inWaiting()>0:
            # Framestart byte
            framestart_byte = ser.read(1)
            myCheckSum = 0
            myCheckSum += struct.unpack('B', framestart_byte)[0]
            print('Framestart byte:\t', framestart_byte)
            
            # Sensor module type
            sensor_module_type = ser.read(1)
            sensor_module_type = struct.unpack('B', sensor_module_type)[0]
            print('Sensor module type:\t', sensor_module_type)
            myCheckSum += sensor_module_type
            
            # Number of payload bytes
            number_of_payloads = ser.read(1)
            number_of_payloads = struct.unpack('B', number_of_payloads)[0]
            print('Number of payload bytes:', number_of_payloads)
            myCheckSum += number_of_payloads          

            # Payloads & values
            number_of_values = int(number_of_payloads/4)
            value = []
            for i in range(0, number_of_values):
                n = i+1
                payload = ser.read(4)
                myCheckSum += struct.unpack('B', payload[0])[0]
                myCheckSum += struct.unpack('B', payload[1])[0]
                myCheckSum += struct.unpack('B', payload[2])[0]
                myCheckSum += struct.unpack('B', payload[3])[0]
                value.append(struct.unpack('f', payload)[0])
                print('payload', n, ':\t\t', payload, 'value', n, ': ', value[i])
            
            # checksum
            checksum = ser.read(1)
            checksum = struct.unpack('B', checksum)[0]
            last_byte = (myCheckSum >>0) & 0xFF   # get last byte of myChecksum
            myCheckSum = 255-last_byte            # subtract last byte from 0xff
            if checksum == myCheckSum:
                print('Checksum OK')
            else:
                print('Checksum not OK')
            print('\n\n')
        else:
            time.sleep(0.1)
except KeyboardInterrupt:
    ser.close()
    print('bye')

