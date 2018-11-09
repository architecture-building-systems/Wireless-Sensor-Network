<?php
// Helper functions
    // get_number_of_values()
    // grabs unit of measurement value by sensorModuleType form database
    function get_number_of_values($sensor_module_type){
    // Database information
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php'); 
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
?>