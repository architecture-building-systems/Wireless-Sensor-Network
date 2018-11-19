<!------------------------------------------------------------------------------
External_access_v1.php
This script shows an overview of the sensor nodes and whether there is fresh
data available or not. The table shows all sensor nodes belonging to the access token.
This script is derived from Isituprightnow_v4.php.
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
/ Parse GET-parameters
/************************************************/
// Set gateway id
    $token = mysqli_real_escape_string($conn, $_GET["token"]);
    if (isset($_GET["gateway_id"])) {
        $gateway_id = mysqli_real_escape_string($conn, $_GET["gateway_id"]);
    }
    else {
        $gateway_id = 1; // If there is no gateway id available as GET-Parameter, set it to 1
    }
/************************************************
/ Echo title of site
/************************************************/
   echo "<div>";
   echo "<h1><a href=\"?\">Is it up right now?</a></h1>\n";
   echo date("d.m.Y H:i:s", time());
   echo "</div>\n";
 
/************************************************
/ Check access token
/************************************************/
    // Look for access token in database
    $type = "read";
    $access = check_token($token, $type);
    
    // Abort script if access is not granted.
    if (!$access){
      echo("<div> Access token is invalid. </div>");
      exit();
    } 
 
/************************************************
/ Get node ids corresponding to the access token
/************************************************/
    // Get campaign id from token
    $sql = "SELECT `campaign_id` FROM `wsn_access` WHERE `token`='$token' ORDER BY id DESC LIMIT 1";
    $result = $conn->query($sql);
    $row = $result->fetch_array(MYSQLI_ASSOC);
    $campaign_id = $row["campaign_id"];
       
    // Get start date and end date from campaign id
    $sql = "SELECT `start_date`, `end_date` FROM `wsn_measurement_campaign` WHERE `id`='$campaign_id' ORDER BY id DESC LIMIT 1";
    $result = $conn->query($sql);
    $row = $result->fetch_array(MYSQLI_ASSOC);
    $start_date = $node_id_list = $row["start_date"];
    $end_date = $node_id_list = $row["end_date"];
    
    // Get node id list from campaign id
    $sql = "SELECT `node_id_list` FROM `wsn_measurement_campaign` WHERE `id`='$campaign_id' ORDER BY id DESC LIMIT 1";
    $result = $conn->query($sql);
    $row = $result->fetch_array(MYSQLI_ASSOC);
    $node_id_list = $row["node_id_list"];
    $node_id_list = explode(', ', $node_id_list);                                // Convert string to array of strings
    $node_id_list = array_map('intval', $node_id_list);                          // Cast strings in array to integers in array


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

// Create list of node ids
    $node_list = $node_id_list;

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