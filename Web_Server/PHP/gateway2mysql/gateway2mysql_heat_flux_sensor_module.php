<?php
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

?>
