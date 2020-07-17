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
        echo("<br>Error: Could not find \"sensor_moudle_type\" in database.<br>");
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
        
 mail($rechoeceiver, $subject, $message, $headers);
    }


/************************************************
/ Get duration since gateway with id=$id transmitted measured data to the database
/ Input: $id:   gateway id
/        $conn: database connection
/ Output: duration since last databse entry from gateway with id $id in minutes
/************************************************/
    function checkGateway($id, $conn){
        $sql = "SELECT MAX(time) as date FROM wsn_input WHERE gateway_id=$id AND `time`>(DATE_SUB(CURDATE(), INTERVAL 1 WEEK)) GROUP BY gateway_id";
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
        $sql = "SELECT MAX(time) as date FROM wsn_input WHERE node_id=$id AND `time`>(DATE_SUB(CURDATE(), INTERVAL 1 WEEK)) GROUP BY node_id";
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
/ To Do: Use joins for sql statement
/        Combine getSEnsorType, get_entity to get_meta_data
/************************************************/
    function getSensorType($id, $conn){
        $sql = "SELECT sensorModuleType FROM wsn_input WHERE node_id=$id AND SensorModuleType!=20 AND SensorModuleType!=13 ORDER BY id DESC LIMIT 1";
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
/ Input: $node_id:   id of sensor node
         $entity_nr: which entity of a particular sensor module type is requested, e.g. SHT31 has two entities: 1. air temperature, 2. relative humidity)
/ Output: Entity as string
/ To Do: Use joins for sql statement
/************************************************/
    function get_entity($node_id, $entity_nr, $heartbeat=False){
    // Include configuration file
        include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');

    // Establish connection to database
        $conn = new mysqli($db_server, $db_username, $db_password, $db_name);

    // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }

    // Get the sensor module type
    if ($heartbeat == False){
        // Get the sensor module type of the last entry from the sensor node with id $node_id
        $sql = "SELECT sensorModuleType FROM wsn_input WHERE node_id=$node_id ORDER BY id DESC LIMIT 1";
        $result = $conn->query($sql);
        $temp = $result->fetch_array(MYSQLI_ASSOC);
        $sensorModuleType = (int)$temp["sensorModuleType"];
    } else {
        $sensorModuleType = 20;
    }


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

/************************************************
/ Check vadility of access token
/ Input:  $token: access token as string, e.g. 23bsxmdzurk7qt8x
          $type:  type of access as string, e.g. "read", "insert"
/ Output: $access: boolean for access, e.g. False for access not granted, True for access granted
/************************************************/
    function check_token($token, $type){
    // Include configuration file
        include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');

    // Establish connection to database
        $conn = new mysqli($db_server, $db_username, $db_password, $db_name);

    // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }

    // Look for access token in database
        $sql = "SELECT `$type` FROM `wsn_access` WHERE `token`='$token' ORDER BY `id` DESC LIMIT 1";
        $result = $conn->query($sql);
        $row = $result->fetch_array(MYSQLI_ASSOC);

    // End script if access token could not be found
        if (empty($row)){
          echo("Access token not found.");
          return False;
        }

    // End script if access token does not grant read rights
        $read_permission = $row[$type];
        if (!$read_permission){
          echo("Access token does not grant read access.");
          return False;
        }

    // End function if acces token is valid
        if ($read_permission){
          return True;
        }

    // If functions ends here something went wrong
        echo("Something went wrong.");
        return False;
    }


/************************************************
/ Get the heartbeat information from id=$id
/ Input: $id:   sensor node id
/        $conn: database connection
/ Output: RSSI and battery voltage in array from the last database entry from the sensor node with id=$id
/************************************************/
    function getHeartbeat($id, $conn){
        $sql = "SELECT value0, value1 FROM wsn_input WHERE node_id=$id AND SensorModuleType=20  AND `time`>(DATE_SUB(CURDATE(), INTERVAL 1 WEEK)) ORDER BY id DESC LIMIT 1";
        $result = $conn->query($sql);
        if ($result->num_rows > 0) {
            $row = $result->fetch_assoc();
            $batteryVoltage = $row["value0"];
            $RSSI = $row["value1"];

            // Convert RSSI to perecentages
            $RSSI_max = -40;
            $RSSI_min = -75;
            $RSSI_percentage = 100/($RSSI_max-$RSSI_min)*($RSSI-$RSSI_min);
            $RSSI_percentage = round($RSSI_percentage,0);
            // Convert battery voltage to persentages
            $battery_percentage = 0;
                 if ($batteryVoltage>=4.20){ $battery_percentage = 100;}
            else if ($batteryVoltage>=4.111){ $battery_percentage = 95;}
            else if ($batteryVoltage>=4.060){ $battery_percentage = 90;}
            else if ($batteryVoltage>=4.020){ $battery_percentage = 85;}
            else if ($batteryVoltage>=3.978){ $battery_percentage = 80;}
            else if ($batteryVoltage>=3.942){ $battery_percentage = 75;}
            else if ($batteryVoltage>=3.909){ $battery_percentage = 70;}
            else if ($batteryVoltage>=3.880){ $battery_percentage = 65;}
            else if ($batteryVoltage>=3.836){ $battery_percentage = 60;}
            else if ($batteryVoltage>=3.807){ $battery_percentage = 55;}
            else if ($batteryVoltage>=3.784){ $battery_percentage = 50;}
            else if ($batteryVoltage>=3.767){ $battery_percentage = 45;}
            else if ($batteryVoltage>=3.755){ $battery_percentage = 40;}
            else if ($batteryVoltage>=3.745){ $battery_percentage = 35;}
            else if ($batteryVoltage>=3.740){ $battery_percentage = 30;}
            else if ($batteryVoltage>=3.719){ $battery_percentage = 25;}
            else if ($batteryVoltage>=3.701){ $battery_percentage = 20;}
            else if ($batteryVoltage>=3.685){ $battery_percentage = 15;}
            else if ($batteryVoltage>=3.648){ $battery_percentage = 10;}
            else if ($batteryVoltage>=3.629){ $battery_percentage =  5;}
            else if ($batteryVoltage>=3.400){ $battery_percentage =  0;}

            $heartbeat = array("batteryVoltage" => $battery_percentage, "RSSI" => $RSSI_percentage);
            return $heartbeat;
        } else {
            $heartbeat = array("batteryVoltage" => -1, "RSSI" => -1);
            return $heartbeat;
        }
    }


/************************************************
/ Generate a new access token that is not already present in the database
/ Input: -
/ Output: access token (string)
/************************************************/
    function generate_token($conn){
        $token_is_unique = FALSE;
        while($token_is_unique==FALSE) {
            // Generate string
            $alphabet = "abcdefghijklmnopqrstuwxyzZ0123456789";
            $length = 16;
            for ($i = 0; $i < $length; $i++) {
                $n = rand(0, strlen($alphabet)-1);    // Not cryptographically secure according to https://stackoverflow.com/questions/6101956/generating-a-random-password-in-php/31284266#31284266
                $token[$i] = $alphabet[$n];
            }
            $token = implode($token);

            // Check database for
            $sql = "SELECT token FROM wsn_access WHERE token=$token";
            $result = $conn->query($sql);
            if ($result->num_rows == 0) {
                $token_is_unique = TRUE;
            }
        }
        // Return token
        return $token;
    }
?>