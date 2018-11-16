<?php
/********************************************************************************************************************
/ gateway2mysql.php
/ This script takes the payload (bytes in hexadecimal format) from the gateway disaggregates it 
/ and inserts the values into the database.
/ Author: Mario Frei (2018)
/********************************************************************************************************************/
/************************************************
/ Debugging
/************************************************/
    echo "Beginning of File<br>";                                                         // Just for debugging

/************************************************
/ Configurations
/************************************************/
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/gateway2mysql_functions.php');                // Include helper functions
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');                                 // Include configuration file

/************************************************
/ Check all GET-parameters from the gateway
/************************************************/
// Received data from gateway
    $payload   = $_GET['payload'];                                                        // XBee payload in hexadecimal format   
    $comment   = $_GET['comment'];                                                        // Comment (e.g. Reset)
    if (empty($_GET['g_id'])){                                                            // Gateway ID
       $gateway_id = -1;
    } else {
        $gateway_id= $_GET['g_id'];                                                     
    }
    
    $ip        = $_SERVER['REMOTE_ADDR'];                                                 // Gateway ip 

/************************************************
/ Write data to file (as backup)
/************************************************/
    $fh = fopen("backup.csv", "a+");                                                       // Open file with curser at the end of the file
    fwrite($fh, date("d.m.Y H:i:s"));                                                      // Write to file, tabs as seperator
    fwrite($fh, "\t");
    fwrite($fh, time());                                                                   
    fwrite($fh, "\t");
    fwrite($fh, $payload);
    fwrite($fh, "\t");
    fwrite($fh, $ip);
    fwrite($fh, "\n");                                                                     // Use new line for every set of data
    fclose($fh);                                                                           // Close file

/************************************************
/ Disaggregate payload
/************************************************/
    //$payload   = "1.0.2.1.64.66.D4.41.B2.13.38.42.";                                    // Example of a raw payload as received from the gateway, bytes are separated by a period
    //$payload   = " 1. 0. 2. 1.64.66.D4.41.B2.13.38.42.";                                // Raw payload with two letters per bytes
    //              01.00.01.01.AC.47.CF.41.01.64.68.42.                                  // Raw payload with inserted zeros for bytes below the value of 16 (10)
    //               0  1  2  3  4  5  6  7  8  9 10 11                                   // Numeration of bytes above
    
// Convert payload form hexadecimal values to decimal values
    $payloadArray = explode(".", $payload);
    foreach ($payloadArray as $payloadArrayItem) {
        $payloadBytes[] = hexdec($payloadArrayItem);
    }
    
// Depending on the payload use a different script for the disaggregation
    if ($payloadBytes[0]==255 && $payloadBytes[1]==255 && $payloadBytes[2]==255 && $payloadBytes[3]==255){
        include 'gateway2mysql_battery_warning.php';
    }
    else if ($payloadBytes[3]==7){
        include 'gateway2mysql_heat_flux_sensor_module.php';
    }
    else {
        include 'gateway2mysql_standard_sensor_module.php';
    }


/************************************************
/ Insert data into database
/************************************************/
// Connect to database
    $conn = mysqli_connect($db_server, $db_username, $db_password, $db_name);              // Connect to database

    if ($conn->connect_error)                                                              // Check database connection
    {
      trigger_error('Database connection failed: '  . $conn->connect_error, E_USER_ERROR);
    }
    
// Insert  raw payload into database
    $sql = "INSERT INTO wsn_input_raw (payload) VALUES ('$payload')";                      
    if($conn->query($sql) === false) 
    {
      trigger_error('Wrong SQL: ' . $sql . ' Error: ' . $conn->error, E_USER_ERROR);
    }
    
// Insert disaggregated payload into database
    $sql = "INSERT INTO wsn_input (node_id, sensorModuleType, ";                           // Example: $sql = "INSERT INTO wsn_input (node_id, sensorModuleType, value0, value1, ip) VALUES ($node_id, $sensorModuleType, $measurementValues[0], $measurementValues[1], '$ip')";
    for ($i=0; $i<sizeof($measurementValues); $i++){
        $sql .= "value$i, ";
    }
    $sql .= "ip, gateway_id, comment) VALUES ($node_id, $sensorModuleType, ";
    
    for ($i=0; $i<sizeof($measurementValues); $i++){
        $sql .= "$measurementValues[$i], ";
    }
    $sql .= "'$ip', $gateway_id, '$comment')";
    echo $sql;
    if($conn->query($sql) === false) 
    {
        trigger_error('Wrong SQL: ' . $sql . ' Error: ' . $conn->error, E_USER_ERROR);
    }
                                                                                          
// Close db connection
    $conn->close();                                                                        
    
    echo "</br>\n";
    echo "End of File";                                                                    // Just for debugging*/
?>
