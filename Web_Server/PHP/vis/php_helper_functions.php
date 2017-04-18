<?
// Debugging
    ini_set('error_reporting','E_ALL');
    ini_set('display_errors', 1);    
    //echo "Beginning of File<br>";  
    
    
// Get entitity of DB entries

function get_entity($node_id, $entity_nr){
    $servername = "localhost";   // Add database url here
    $username   = "";            // Add database user name here
    $password   = "";            // Add database password here
    $dbname     = "ddr";         // Add database name here

    $conn = new mysqli($servername, $username, $password,$dbname);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    // Select data
    $sql = "SELECT sensorModuleType FROM wsn_input WHERE node_id=$node_id ORDER BY id DESC LIMIT 1";
    $result = $conn->query($sql);
    $temp = $result->fetch_array(MYSQLI_ASSOC);
    $sensorModuleType = (int)$temp["sensorModuleType"];
    
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
