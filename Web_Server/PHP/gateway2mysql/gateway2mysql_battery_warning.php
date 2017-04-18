<?php
    // Node id
    $node_id = $payloadBytes[4] + 256*$payloadBytes[5];
    $sensorModuleType = -1;
    $comment = "Low battery warning";
    
    // Battery voltage 
    $temp =  pack("c*", $payloadBytes[6], $payloadBytes[7], $payloadBytes[8], $payloadBytes[9]);
    $temp2= unpack("f", $temp);
    $battery_voltage = $temp2[1];
    $measurementValues[0] = $battery_voltage;
    
    // Send low battery warning mail
    low_battery_warning_mail($node_id);


?>