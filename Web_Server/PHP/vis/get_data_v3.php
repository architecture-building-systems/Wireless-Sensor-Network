<?php
/********************************************************************************************************************
/ get_data_v3.php
/ This script takes the node id and fetches the most recent timeseries data from the database.
/ The data is output as json. The json data is used to plot the timeseries using the metrics graphics framework.
/ Author: Mario Frei (2018)
/ Source: http://www.w3schools.com/php/php_mysql_select.asp
/********************************************************************************************************************/

/************************************************
/ Include configuration file
/************************************************/
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/php_helper_functions.php');


/************************************************
/ Create database  connection
/************************************************/
    $conn = new mysqli($db_server, $db_username, $db_password,$db_name);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 


/************************************************
/ Parse GET-parameters
/************************************************/
    $node_id = mysqli_real_escape_string($conn, $_GET["node_id"]);
    $token = mysqli_real_escape_string($conn, $_GET["token"]);
    
    
/************************************************
// Check access token for validity
/************************************************/
    // Look for access token in database
    $type = "read";
    $access = check_token($token, $type);
    
    // Abort script if access is not granted.
    if (!$access){
      exit();
    } 
    
    // Continue if access token is valid

    
/************************************************
// Check if node id can be accessed with token
/************************************************/
    // Get campaign id from token
    $sql = "SELECT `campaign_id` FROM `wsn_access` WHERE `token`='$token' ORDER BY id DESC LIMIT 1";
    $result = $conn->query($sql);
    $row = $result->fetch_array(MYSQLI_ASSOC);
    $campaign_id = $row["campaign_id"];
    
    if ($campaign_id == '*'){
      $start_date = '2001-01-01 00:00:00';                                         // Default date in PhpMyAdmin
      $end_date = 'date("Y-m-d H:i:s")';                                           // Current date and time
    } 
    else {     
      // Get start date and end date from campaign id
      $sql = "SELECT `start_date`, `end_date` FROM `wsn_measurement_campaign` WHERE `id`='$campaign_id' ORDER BY id DESC LIMIT 1";
      $result = $conn->query($sql);
      $row = $result->fetch_array(MYSQLI_ASSOC);
      $start_date = $node_id_list = $row["start_date"];
      $end_date = $node_id_list = $row["end_date"];
      if ($end_date == "0001-01-01 00:00:00"){                                     // The default PhpMyAdmin date needs changed
      $end_date = 'date("Y-m-d H:i:s")';                                           // Current date and time
      }
      
      // Get node id list from campaign id
      $sql = "SELECT `node_id_list` FROM `wsn_measurement_campaign` WHERE `id`='$campaign_id' ORDER BY id DESC LIMIT 1";
      $result = $conn->query($sql);
      $row = $result->fetch_array(MYSQLI_ASSOC);
      $node_id_list = $row["node_id_list"];
      $node_id_list = explode(', ', $node_id_list);                                // Convert string to array of strings
      $node_id_list = array_map('intval', $node_id_list);                          // Cast strings in array to integers in array
      
      $id_in_array = in_array($node_id, $node_id_list);
      if (!$id_in_array){
      	echo("Token does not grant the right to access node_id=$node_id");
        exit();
      }
    }
    
    
/************************************************
// Select data from database
/************************************************/
    // Get sensor module type
    $sql = "SELECT sensorModuleType FROM wsn_input WHERE node_id=$node_id ORDER BY id DESC LIMIT 1";
    $result = $conn->query($sql);
    $row = $result->fetch_array(MYSQLI_ASSOC);
    $sensorModuleType = $row["sensorModuleType"];
    
    // Get number of measurement entries of sensor module type
    $sql = "SELECT number_of_values FROM wsn_sensor_module_type WHERE id=$sensorModuleType ORDER BY id DESC LIMIT 1";
    $result = $conn->query($sql);
    $row = $result->fetch_array(MYSQLI_ASSOC);
    $number_of_values = $row["number_of_values"];
    
    // Get measurement values
    $sql = "SELECT time as date, value0 , value1, value2, value3, value4 
            FROM wsn_input
            WHERE node_id=$node_id
            AND sensorModuleType=$sensorModuleType
            AND time>'$start_date' 
            AND time<'$end_date'
            ORDER BY id DESC 
            LIMIT 150";
    $result = $conn->query($sql);
    $conn->close();

    if ($result->num_rows > 0) {
        // Read data of each row
        while($row = $result->fetch_array(MYSQLI_ASSOC)) {      
            for ($i=0; $i<$number_of_values; $i++ ){
                $myValueArray[$i] [] = array("date"=>$row["date"], "value"=>(float)$row["value$i"]);
            }
        }
    } else {
        echo "0 results";
    }
    
    
/************************************************
// Output json
/************************************************/
    header("Content-Type: application/json;charset=utf-8");
    echo json_encode($myValueArray);

?>
