<?php
    // Source: http://www.w3schools.com/php/php_mysql_select.asp

    // Include configuration file
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');
    
    $node_id = $_GET["node_id"];

    // Create connection
    $conn = new mysqli($db_server, $db_username, $db_password,$db_name);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 
    //echo "Connected successfully <br/>\n";

    // Select data
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
    $sql = "SELECT time as date, value0 , value1, value2, value3, value4 FROM wsn_input WHERE node_id=$node_id AND sensorModuleType=$sensorModuleType ORDER BY id DESC LIMIT 150";
    $result = $conn->query($sql);
    $conn->close();
    
    if ($result->num_rows > 0) {
        // output data of each row
        while($row = $result->fetch_array(MYSQLI_ASSOC)) {
            //$value0Array [] = array("date"=>$row["date"], "value"=>(float)$row["value0"]);      
            for ($i=0; $i<$number_of_values; $i++ ){
                $myValueArray[$i] [] = array("date"=>$row["date"], "value"=>(float)$row["value$i"]);
            }
        }
    } else {
        echo "0 results";
    }

    // Output json
    header("Content-Type: application/json;charset=utf-8");
    echo json_encode($myValueArray);

?>
