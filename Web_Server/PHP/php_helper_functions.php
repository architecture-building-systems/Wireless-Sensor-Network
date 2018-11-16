<?
/********************************************************************************************************************
/ php_helper_functions.php
/ This script contains php functions, which were frequently used in other scripts
/ Author: Mario Frei (2018)
/********************************************************************************************************************/

/************************************************
/ get_number_of_values()
/ Get the number of values measured by a certain sensor module type
/ Input: $sensor_module_type: the type of the sensor module (e.g. 4 for TSL2561)
/ Output: number of values: the number of measured values by a certains sensor module type (e.g. 1 for TSL2561 (lux))
/************************************************/
    function get_number_of_values($sensor_module_type){
// Connect to database
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php'); 
    $conn = mysqli_connect($db_server, $db_username, $db_password, $db_name);
    
// Get the data
    $sql = "SELECT number_of_values FROM wsn_sensor_module_type WHERE id=$sensor_module_type LIMIT 1";
    $result = $conn->query($sql);        
    
    if ($result->num_rows > 0) {
        $row = $result->fetch_array(MYSQLI_ASSOC);
        $nov = ($row["number_of_values"]);
        $nov = (int)$nov[0];
        return $nov;
    } 
    else {
        return -1;
    } 
     $conn->close();    
    }

/************************************************
/ low_battery_warning_mail(): sends a mail with a low battery warning notification to the admin.
/ Input: $node_id: id of sensor node
/ Output: None
/************************************************/
    function low_battery_warning_mail($node_id){
        include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php'); 
        
        $receiver  = $e_mail_address_admin;
        
        $subject  = "Low Battery Warning of Node $node_id";
        
        $message  = "The Battery of node ";
        $message .= "<a href=\"http://sustain.arch.ethz.ch/wsn/vis/graph_db_v1.php?node_id=$node_id\">$node_id</a> ";
        $message .= "has reached the minimum voltage threshhold.</br>\n"; 
        $message .= "The node has been shut down.</br>\n";
        $message .= " More information can be found <a href=\"http://sustain.arch.ethz.ch/wsn/vis/isituprightnow_v2.php\">here</a>.";
        
        $headers  = "MIME-Version: 1.0" . "\r\n";
        $headers .= "Content-type:text/html;charset=UTF-8" . "\r\n";
        $headers .= "From:".$e_mail_address_admin;
        
        echo mail($receiver, $subject, $message, $headers);
    }


/************************************************
/ Get duration since gateway with id=$id transmitted measured data to the database
/ Input: $id:   gateway id
/        $conn: database connection
/ Output: duration since last databse entry from gateway with id $id in minutes
/************************************************/
    function checkGateway($id, $conn){
        $sql = "SELECT MAX(time) as date FROM wsn_input WHERE gateway_id=$id AND `time`>(DATE_SUB(CURDATE(), INTERVAL 1 MONTH)) GROUP BY gateway_id";
        $result = $conn->query($sql);        
        if ($result->num_rows > 0) {
            $row = $result->fetch_array(MYSQLI_ASSOC);
            $seconds = time()-strtotime($row["date"]);
            $minutes = $seconds/60;
            return round($minutes, 2);
        } else {
            return -1;
        } 
    }

/************************************************   
/ Check duration since sensor node transmitted measured data to the database
/ Input: $id:   sensor node id
/        $conn: database connection
/ Output: duration since last databse entry from sensor node with id $id in minutes
/************************************************/
    function checkNode($id, $conn){
        $sql = "SELECT MAX(time) as date FROM wsn_input WHERE node_id=$id AND `time`>(DATE_SUB(CURDATE(), INTERVAL 1 MONTH)) GROUP BY node_id";
        $result = $conn->query($sql);        
        if ($result->num_rows > 0) {
            $row = $result->fetch_array(MYSQLI_ASSOC);
            $seconds = time()-strtotime($row["date"]);
            $minutes = $seconds/60;
            return round($minutes, 2);
        } else {
            return -1;
        } 
    }

/************************************************
/ Get the sensor module type from the last database entry from the sensor node with id=$id
/ Input: $id:   sensor node id
/        $conn: database connection
/ Output: sensor module type from the last database entry from the sensor node with id=$id
/************************************************/
    function getSensorType($id, $conn){
        $sql = "SELECT sensorModuleType FROM wsn_input WHERE node_id=$id ORDER BY id DESC LIMIT 1";
        $result = $conn->query($sql);    
        if ($result->num_rows > 0) {
            $row = $result->fetch_assoc();
            $sensorModuleType = $row["sensorModuleType"];
        } else {
            return -1;
        }
        
        $sql = "SELECT description FROM wsn_sensor_module_type WHERE id=$sensorModuleType ORDER BY id DESC LIMIT 1";
        $result = $conn->query($sql);        
        if ($result->num_rows > 0) {
            $row = $result->fetch_assoc();
            return $row["description"];
        } else {
            return -1;
        } 
    }


/************************************************
/ Get entitity of DB entries
/ Entity is the description of the timeseries data, e.g. temperature, CO2 concentration, etc.
/ Input: $node_id:   id if sensor node
         $entity_nr: which entity of a particular sensor module type is requested, e.g. SHT31 has two entities: 1. air temperature, 2. relative humidity)
/ Output: Entity as string
/************************************************/
function get_entity($node_id, $entity_nr){
// Include configuration file
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');
   // echo ("mimi");
// Establish connection to database
    $conn = new mysqli($db_server, $db_username, $db_password, $db_name);

// Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

// Get the sensor module type of the last entry from the sensor node with id $node_id
    $sql = "SELECT sensorModuleType FROM wsn_input WHERE node_id=$node_id ORDER BY id DESC LIMIT 1";
    $result = $conn->query($sql);
    $temp = $result->fetch_array(MYSQLI_ASSOC);
    $sensorModuleType = (int)$temp["sensorModuleType"];

// Get the entity number $entitity_nr from the sensor module type
    $sql = "SELECT entity FROM wsn_sensor_module_type WHERE id=$sensorModuleType LIMIT 1";
    $result = $conn->query($sql);
    $conn->close();

    $row = $result->fetch_array(MYSQLI_ASSOC);
    $temp = $row["entity"];
    $temp = explode(",", $temp);
    if ($entity_nr < sizeof($temp)){
        return $temp[$entity_nr];
    } 
    else {
        return "";
    }
}
?>