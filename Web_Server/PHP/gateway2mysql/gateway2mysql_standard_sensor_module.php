<?php
/********************************************************************************************************************
/ gateway2mysql_standard_sensor_module.php
/ This script takes the payload message from sensor nodes with standard sensor modules.
/ This script get called in php2mysql.php
/ Idealy, this would be the only necessary script for the disaggregation of the payload.
/ Author: Mario Frei (2018)
/********************************************************************************************************************/

// Communication module ID ( $payloadBytes[0:1] )
    $node_id = $payloadBytes[0] + 256*$payloadBytes[1];
 
// Number of measurements ( $payloadBytes[2] )
    $numOfMeas = $payloadBytes[2]+1;                                                       //+1 because it seams to be wrong in the arduino script
    echo "numOfMeas=";
    echo $numOfMeas;
    echo "<br>\n";
    
// Sensor module type ( $payloadBytes[3] )
    $sensorModuleType = $payloadBytes[3];
    
// Measurement value ( $payloadBytes[4:end] )
    $numOfValues = get_number_of_values($sensorModuleType);
    $measurementValues = array();
    for ($i= 0; $i<$numOfValues; $i++){
        $temp =  pack("c*", $payloadBytes[4+4*$i], $payloadBytes[5+4*$i], $payloadBytes[6+4*$i], $payloadBytes[7+4*$i]);
        $temp2= unpack("f", $temp);
        $measurementValues[$i] = $temp2[1];
        echo "measurementValues[$i]=";
        echo $measurementValues[$i];
        echo "<br>\n";
    }

// Go back to gateway2mysql.php
?>