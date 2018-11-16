<!------------------------------------------------------------------------------
Isituprightnow_v4.php
This script shows an overview of the sensor nodes and whether there is fresh
data available or not. One table shows the status of all gateways. The second
table shows all sensor nodes of one gateway.
Author: Mario Frei (2018) 
 ------------------------------------------------------------------------------>
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
/************************************************
/ Configurations
/************************************************/
    $time_start = microtime(true);          
    $number_of_gateways     = 8;  // Maximal number of gateways to show in the table (starting from 1)
    $number_of_sensor_nodes = 18; // Maximal number of sensor node to show in the table (starting from xx1)
    $accepted_delay         = 16; // Maximal accepted delay in minutes for which the cell is still highlighted green
    include_once($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');
    include_once($_SERVER['DOCUMENT_ROOT'].'/wsn/php_helper_functions.php');
    
/************************************************
// DB connection 
/************************************************/
    // Create connection
    $conn = new mysqli($db_server, $db_username, $db_password, $db_name);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 

/************************************************
/ Echo title of site
/************************************************/
   echo "<div>";
   echo "<h1><a href=\"?\">Is it up right now?</a></h1>\n";
   echo date("d.m.Y H:i:s", time());
   echo "</div>\n";
 
/************************************************
/ Echo gateway table
/************************************************/  
    echo "<div id=\"mytables\">";
    echo "<h2>Gateways</h2>\n";
    echo "<table>";
    echo "<tr>";
    echo "<th>Gateway ID</th>";
    echo "<th>Minutes since last entry</th>";
    echo "<th>Status</th>";
    echo "\n";
    echo "</tr>";
    for ($i=1; $i<=$number_of_gateways; $i++)
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

/************************************************
/ Echo node table
/************************************************/  
// Echo table headers
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

// Set gateway id
    if (isset($_GET["gateway_id"])) {
        $gateway_id = $_GET["gateway_id"];
    }
    else {
        $gateway_id = 1; // If there is no gateway id available as GET-Parameter, set it to 1
    }

// Create list of node ids
    $node_list = array();
    $pan_id = 100*$gateway_id;
    for ($i=1; $i<=$number_of_sensor_nodes; $i++){
        $node_list[] = $pan_id + $i;
    }

// Output information for each node listed in the node id list from above, row by row
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
        echo "<td> <a href=\"graph_db_v1.php?node_id=$node_id\">$node_id</a></td>";
        //echo "<td> <a href=\"http://sustain.arch.ethz.ch/wsn/vis/graph_db_v1.php?node_id=$node_id\">$node_id</a></td>";
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
    
// Echo execution time
    $time_end = microtime(true);
    $execution_time = round(($time_end - $time_start),1);
    echo "<div id=\"exectime\">Total Execution Time: ".$execution_time." Seconds </div>";
?>
</body>
</html>