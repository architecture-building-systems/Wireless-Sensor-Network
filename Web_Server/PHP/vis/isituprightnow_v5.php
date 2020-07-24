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
            font-family: "Helvetica";
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
/ Check all GET-parameters from the gateway
/************************************************/
// Received data from gateway
    $token     = mysqli_real_escape_string($conn, $_GET['token']);                        // Access token (escaped for SQL)
    if (isset($_GET["gateway_id"])){                                                      // Get gateway ID (escaped for SQL)
        $gateway_id_url = mysqli_real_escape_string($conn, $_GET["gateway_id"]);
    }
    else {
        $gateway_id_url = 1; // If there is no gateway id available as GET-Parameter, set it to 1
    }

    if (isset($_GET["campaign_id"])){                                                    // Get campaign ID (escaped for SQL)
        $campaign_id_url = mysqli_real_escape_string($conn, $_GET["campaign_id"]);
    }
    else {
        $campaign_id_url = -1; // If there is no campaign id available as GET-Parameter, set it to -1
    }
/************************************************
/ Check access token for validity
/************************************************/
  $type = "read";
  $access = check_token($token, $type);
    // Abort script if access is not granted.
    if (!$access){
      exit();
    }

/************************************************
/ Echo title of site
/************************************************/
   echo "<div>";
   echo "<h1><a href=\"?\">Is it up right now?</a></h1>\n";
   echo date("d.m.Y H:i:s", time());
   echo "</div>\n";
 
/************************************************
/ Echo deployment table
/************************************************/
    echo "<div id=\"mytables\">";
    echo "<h2>Deployments</h2>\n";
    echo "<table>";
    echo "<tr>";
    echo "<th>ID</th>";
    echo "<th>Name</th>";
    echo "<th>Description</th>";
    echo "<th>Nodes</th>";
    echo "<th>Last entry (min)</th>";
    echo "<th>Status</th>";
    echo "\n";
    echo "</tr>";

// Get active campaigns
    $sql = "SELECT `id`,`campaign_name`,`description`, `node_id_list`, `gateway_id` FROM `wsn_measurement_campaign` WHERE `end_date`='0001-01-01 00:00:00' OR `end_date`>NOW() ORDER BY id ASC";
    $result = $conn->query($sql);

    while($row = $result->fetch_array(MYSQLI_ASSOC)){
        $campaign_id = $row["id"];
        $campaign_name = $row["campaign_name"];
        $campaign_description = $row["description"];
        $gateway_id_list =$row["gateway_id"];
        $gateway_id_list = explode(",", $gateway_id_list);
        $gateway_id_list = array_map('intval', $gateway_id_list);

        $node_id_list = $row["node_id_list"];
        $node_id_list = explode(",", $node_id_list);
        $num_nodes = count($node_id_list);

        $timePast = 10000;  // [minutes]
        foreach($gateway_id_list as $id){
            $timePast_temp = checkGateway($id, $conn);
            if ($timePast_temp < $timePast) {
                $timePast = $timePast_temp;
            }
        }
// Output campaign details
        echo "<tr>\n";
        echo "<td>$campaign_id</td>";
        echo "<td><a href=\"?campaign_id=$campaign_id&token=$token\"> $campaign_name </a></td>";
        echo "<td>$campaign_description</td>";
        echo "<td>$num_nodes</td>";
        echo "<td>";
        echo number_format($timePast,0,",","'");
        echo "</td>";
        if ($timePast >-1 && $timePast < 10){
           echo "<td bgcolor=\"#00FF00\">OK</td>";
        } else {
           echo "<td bgcolor=\"#FF0000\">Offline </td>";
        }
        echo "</tr>";
    }
    echo "</table>";
    echo "</div>";

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
        echo "<td> <a href=\"?gateway_id=$i&token=$token\">$i</a></td>";
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
    echo "<th>RSSI</th>";
    echo "<th>Battery</th>";
    echo "\n";
    echo "</tr>";


// Create list of node ids
    if ($campaign_id_url!=-1) {     // Get node_id based on campaign name if campaign id is available
        $node_list = array();
        $sql = "SELECT `node_id_list` FROM `wsn_measurement_campaign` WHERE `id`=$campaign_id_url LIMIT 1";
        $result = $conn->query($sql);
        $row = $result->fetch_array(MYSQLI_ASSOC);
        $node_list = $row["node_id_list"];
        $node_list = explode(",", $node_list);
        $node_list = array_map('intval', $node_list);

    } else {                        // Get node_id based on gateway id
        $node_list = array();
        $pan_id = 100*$gateway_id_url;
        for ($i=1; $i<=$number_of_sensor_nodes; $i++){
            $node_list[] = $pan_id + $i;
        }
    }

// Output information for each node listed in the node id list from above, row by row
    for ($i=0; $i<count($node_list); $i++)
    {
        $node_id = $node_list[$i];

        // Fetch time passed since last entry
        $timePast = checkNode($node_id, $conn);

        // Fetch description of sensor module
        if ($timePast > -1 AND $timePast < $accepted_delay){
            $description = getSensorType($node_id, $conn);
        } else {
            $description = "-";
        }

        // Fetch heartbeat info
        $heartbeat = getHeartbeat($node_id, $conn);
        $RSSI = $heartbeat["RSSI"];
        $batteryVoltage  = $heartbeat["batteryVoltage"];

        // assign background color for heartbeat info
        $RSSI_color = "#CACACA";
             if ($RSSI>55) {$RSSI_color = "#00FF00";}
        else if ($RSSI>30) {$RSSI_color = "#FFFF00";}
        else if ($RSSI>10) {$RSSI_color = "#FFC90E";}
        else if ($RSSI> 0) {$RSSI_color = "#FF0000";}
        $bat_color = "#CACACA";
             if ($batteryVoltage>55) {$bat_color = "#00FF00";}
        else if ($batteryVoltage>30) {$bat_color = "#FFFF00";}
        else if ($batteryVoltage>10) {$bat_color = "#FFC90E";}
        else if ($batteryVoltage> 0) {$bat_color = "#FF0000";}


        // Display table
        echo "<tr>";
        echo "<td> <a href=\"graph_db_v2.php?node_id=$node_id&token=$token\">$node_id</a></td>";
        echo "<td>$description</td>";
        echo "<td>";
        echo number_format($timePast,2,",","'");
        echo "</td>";
        if ($timePast > -1 AND $timePast < $accepted_delay){
           echo "<td bgcolor=\"#00FF00\">OK</td>";
        } else {
          echo "<td bgcolor=\"#FF0000\">Offline</td>";
        }
        echo "<td bgcolor=\"$RSSI_color\"> <a href=\"./graph_db_v2_heartbeat.php?node_id=$node_id&token=$token\">$RSSI%</a></td>";
        echo "<td bgcolor=\"$bat_color\">  <a href=\"./graph_db_v2_heartbeat.php?node_id=$node_id&token=$token\">$batteryVoltage%</a></td>";

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