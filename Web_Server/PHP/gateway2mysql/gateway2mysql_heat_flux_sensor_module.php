<?php
/********************************************************************************************************************
/ gateway2mysql_heat_flux_sensor.php
/ This script takes the payload message sent by sensor node with heat flux sensor modules and disaggregates it.
/ This script get called in php2mysql.php.
/ This script is necessary because the sensor modules for heat flux sensor do not strictly adhere to the payload scheme.
/ Author: Mario Frei (2018)
/********************************************************************************************************************/

    // Communication module ID ( $payloadBytes[0:1] )
    $node_id = $payloadBytes[0] + 256*$payloadBytes[1];


    // Number of measurements ( $payloadBytes[2] )
    $numOfMeas = $payloadBytes[2]+1;                                                       //+1 because it seams to be wrong in the arduino script

    // Sensor module type ( $payloadBytes[3] )
    $sensorModuleType = $payloadBytes[3];


    // Measurement value ( $payloadBytes[4:end] )    
    $temp =  pack("c*", $payloadBytes[4], $payloadBytes[5]);
    $temp2= unpack("s", $temp);
    $measurementValues[0] = $temp2[1]/10;

// Go back to gateway2mysql.php
?>
