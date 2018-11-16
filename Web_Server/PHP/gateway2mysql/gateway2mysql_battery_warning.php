<?php
/********************************************************************************************************************
/ gateway2mysql_battery_warning.php
/ This script takes the battery warning payload message from the gateway disaggregates it.
/ This script get called in php2mysql.php
/ This scirpt is necessary because battery warning messages do not strictly adhere to 
/ the standard payload message scheme. In addition an e-mail is sent to the admin 
/ with a notification about low battery state of the sensor node.
/ Author: Mario Frei (2018)
/********************************************************************************************************************/

/************************************************
/ Get node id
/************************************************/
    $node_id = $payloadBytes[4] + 256*$payloadBytes[5];
    $sensorModuleType = -1;
    $comment = "Low battery warning";
    
/************************************************
/ Get battery voltage
/************************************************/
    $temp =  pack("c*", $payloadBytes[6], $payloadBytes[7], $payloadBytes[8], $payloadBytes[9]);
    $temp2= unpack("f", $temp);
    $battery_voltage = $temp2[1];
    $measurementValues[0] = $battery_voltage;

/************************************************
/ Send low battery warning mail
/************************************************/
    low_battery_warning_mail($node_id);

// Go back to gateway2mysql.php
?>