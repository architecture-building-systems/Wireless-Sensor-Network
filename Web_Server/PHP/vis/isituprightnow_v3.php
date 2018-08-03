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
    $servername = "localhost";   // Add database url here
    $username   = "";            // Add database user name here
    $password   = "";            // Add database password here
    $dbname     = "ddr";         // Add database name here
    
    // Create connection
    $conn = new mysqli($servername, $username, $password,$dbname);

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
   
   
// Node table
    echo "<div id=\"mytables\">";
    echo "<h2>Nodes</h2>";
    echo "<table>";
    echo "<tr>";
    echo "<th></th>";
    echo "<th>1</th>";
    echo "<th>2</th>";
    echo "<th>3</th>";
    echo "<th>4</th>";
    echo "<th>5</th>";
    echo "<th>6</th>";
    echo "<th>7</th>";
    echo "<th>8</th>";
    echo "<th>9</th>";
    echo "<th>10</th>";
    echo "<th>11</th>";
    echo "<th>12</th>";
    echo "<th>13</th>";
    echo "<th>14</th>";
    echo "<th>15</th>";
    echo "<th>16</th>";
    echo "<th>17</th>";
    echo "<th>18</th>";
    echo "</tr>";
    echo "\n";
    echo "<tr>";
    echo "<th></th>";
    echo "<th>SHT31</th>";
    echo "<th>SHT31</th>";
    echo "<th>SHT31</th>";
    echo "<th>W</th>";
    echo "<th>W</th>";
    echo "<th>W</th>";
    echo "<th>CO2</th>";
    echo "<th>DS</th>";
    echo "<th>DS</th>";
    echo "<th>DS</th>";
    echo "<th>DS</th>";
    echo "<th>DS</th>";
    echo "<th>DS</th>";
    echo "<th>HF</th>";
    echo "<th>HF</th>";
    echo "<th>HF</th>";
    echo "<th>E</th>";
    echo "<th>Oil</th>";
    echo "</tr>";
    echo "\n";
    
    
    $nodes_not_installed = array(209, 210, 310, 317, 517, 618, 706, 717, 718, 817, 818);
    
    $accepted_delay = 16; // [minutes]
    for ($i=1; $i<=8; $i++){
        $kit_id=$i*100;
        echo "<tr>";
        echo "<td>$kit_id</td>";
        for ($j=1; $j<=18; $j++){
            $node_id = $kit_id + $j;
            if (in_array($node_id, $nodes_not_installed)){
                echo "<td>n.i.</td>";
            }
            else {
                $timePast = checkNode($node_id, $conn);
                if ($timePast > -1 AND $timePast < $accepted_delay){
                    $description = getSensorType($node_id, $conn);
                } else {
                    $description = "-";
                }
                if ($timePast > -1 AND $timePast < $accepted_delay){
                   echo "<td bgcolor=\"#00FF00\">";
                } else {
                  echo "<td bgcolor=\"#FF0000\">";
                }
                echo "<a href=\"http://sustain.arch.ethz.ch/wsn/vis/graph_db_v1.php?node_id=$node_id\">";
                echo number_format($timePast,2,",","'");
                echo "</a>";
                echo "</td>";
            }
        }
    echo "</tr>\n";
    }   
    echo "</table>";
    echo "<div> n.i.: not installed </div>";
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
