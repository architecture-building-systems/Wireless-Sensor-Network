<?php
// Helper functions
    // get_number_of_values()
    // grabs unit of measurement value by sensorModuleType form database
    function get_number_of_values($sensor_module_type){
    // Database information
    $db_server      = "localhost";   // Add database address here             
    $db_username    = "";  // Add database username here
    $db_password    = "";  // Add database password here
    $db_name        = "";  // Add database name here
    $conn = mysqli_connect($db_server, $db_username, $db_password, $db_name);  // Connect to database
    
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
    
    //low_battery_warning_mail()
    // Sends an e-mail to the $receiver notifying that $node_id has been shut down. 
    function low_battery_warning_mail($node_id){
        $receiver = "user1@url1.com";  // Add receiver e-mail address here
        $sender   = "user2@url2.com";  // Add sender e-mail address here
        $subject  = "Low Battery Warning of Node $node_id";
        $message  = "The Battery of node $node_id ";
        $message .= "has reached the minimum voltage threshhold.</br>\n"; 
        $message .= "The node has been shut down.</br>\n";
        $headers  = "MIME-Version: 1.0" . "\r\n";
        $headers .= "Content-type:text/html;charset=UTF-8" . "\r\n";
        $headers .= "From: $sender";
        echo mail($receiver, $subject, $message, $headers);
    }
?>
