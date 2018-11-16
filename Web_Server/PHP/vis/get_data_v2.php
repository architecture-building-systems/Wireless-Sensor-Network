<?php
    /********************************************************************************************************************
    / get_data_v2.php
    / This script takes the node id and fetches the most recent timeseries data from the database.
    / The data is output as json. The json data is used to plot the timeseries using the metrics graphics framework.
    / Author: Mario Frei (2018)
    / Source: http://www.w3schools.com/php/php_mysql_select.asp
    /********************************************************************************************************************/

    /************************************************
    / Include configuration file
    /************************************************/
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');
    
    /************************************************
    / Create database  connection
    /************************************************/
    $conn = new mysqli($db_server, $db_username, $db_password,$db_name);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 

    /************************************************
    // Select data 
    /************************************************/
    // Get sensor module type
    $node_id = $_GET["node_id"];
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
    $sql = "SELECT time as date, value0 , value1, value2, value3, value4 FROM wsn_input WHERE node_id=$node_id AND sensorModuleType=$sensorModuleType ORDER BY id DESC LIMIT 150";
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
