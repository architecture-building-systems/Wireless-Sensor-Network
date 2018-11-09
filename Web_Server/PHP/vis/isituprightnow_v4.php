<!DOCTYPE html>
<html>
<meta charset="utf-8">
<head>
    <style>
        body, h1, h2, th{
            font-family: "Open Sans";
            font-weight: 110;
        }
        table, th, td {
            border: 1px solid black;
            border-collapse: collapse;
        }
        #mytables {
            width: 500px;
            float:left;
        }    
        #exectime {
          position: absolute;
          bottom: 2%;
          left: 2%;
        }
    </style>
</head>

<body>

<?php
// Debugging
    $time_start = microtime(true);
    //ini_set('error_reporting','E_ALL');
    ini_set('display_errors', 1);    
    //echo "Beginning of File<br>";
           
    
// DB connection
    // Login data    
    include($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');
    // Create connection
    $conn = new mysqli($db_server, $db_username, $db_password, $db_name);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 
    //echo "Connected successfully <br/>\n";
// Echo titles
   echo "<div>";
   echo "<h1><a href=\"?\">Is it up right now?</a></h1>\n";
   echo date("d.m.Y H:i:s", time());
   echo "</div>\n";
   
   // Gateway table
    echo "<div id=\"mytables\">";
    echo "<h2>Gateways</h2>\n";
    echo "<table>";
    echo "<tr>";
    echo "<th>Gateway ID</th>";
    echo "<th>Minutes since last entry</th>";
    echo "<th>Status</th>";
    echo "\n";
    echo "</tr>";
    
    for ($i=1; $i<=8; $i++)
    {
        $timePast = checkGateway($i, $conn);
        echo "<tr>";
        echo "<td> <a href=\"?gateway_id=$i\">$i</a></td>";
        echo "<td>";
        echo number_format($timePast,0,",","'");
        echo "</td>";
        if ($timePast >-1 && $timePast < 10){ 
           echo "<td bgcolor=\"#00FF00\">OK</td>";
        } else {
           echo "<td bgcolor=\"#FF0000\">Offline </td>";
        }
        echo "</tr>\n";
    }
    echo "</table>";
    echo "</div>";
    
// Node table
    echo "<div id=\"mytables\">";
    echo "<h2>Nodes</h2>";
    echo "<table>";
    echo "<tr>";
    echo "<th>Node ID</th>";
    echo "<th>Sensor type</th>";
    echo "<th>Minutes since last entry</th>";
    echo "<th>Status</th>";
    echo "\n";
    echo "</tr>";
    
    //
    if (isset($_GET["gateway_id"])) {
        $gateway_id = $_GET["gateway_id"];
    }
    else {
        $gateway_id = 1;
    }
    $node_list = array();
    $pan_id = 100*$gateway_id;
    for ($i=1; $i<=18; $i++){
        $node_list[] = $pan_id + $i;

    }
    
    $accepted_delay = 16; // [minutes]
    
    for ($i=0; $i<count($node_list); $i++)
    {
        $node_id = $node_list[$i];
        $timePast = checkNode($node_id, $conn);
        if ($timePast > -1 AND $timePast < $accepted_delay){
            $description = getSensorType($node_id, $conn);
        } else {
            $description = "-";
        }
            
        echo "<tr>";
        echo "<td> <a href=\"http://sustain.arch.ethz.ch/wsn/vis/graph_db_v1.php?node_id=$node_id\">$node_id</a></td>";
        echo "<td>$description</td>";
        echo "<td>";
        echo number_format($timePast,2,",","'");
        echo "</td>";
        if ($timePast > -1 AND $timePast < $accepted_delay){
           echo "<td bgcolor=\"#00FF00\">OK</td>";
        } else {
          echo "<td bgcolor=\"#FF0000\">Offline</td>";
        }
        echo "</tr>\n";
    }
    echo "</table>";
    echo "</div>";
    
// Close db-connection    
    $conn->close();
    
// Functions
    // Check time since gateway with id=$id entered a 
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
    $time_end = microtime(true);
    $execution_time = round(($time_end - $time_start),1);
    echo "<div id=\"exectime\">Total Execution Time: ".$execution_time." Seconds </div>";
    //echo "<br>End of file.";
?>
</body>
</html>