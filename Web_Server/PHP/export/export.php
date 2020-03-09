<?php
/************************************************
 * Scirpt to retrieve measurement data over web interface
 * access: http://www.[YOUR URL].com/export.export.php?node_id=[YOUR NODE ID]&token=[YOUR TOKEN]&campaign_name=[YOUR CAMPAIGN NAME]&campaign_description=[YOUR CAMPAIGN DESCRIPTION]&date_start=[YOUR START DATE]&date_end=[YOUR END DATE]
 * Output: json formatted measruement data including meta data
[
   [
      [
         {
            "node_id":801,
            "units":[
               "\u00b0C",
               "%"
            ],
            "description":[
               "SHT31"
            ],
            "entities":[
               "air temperature (indoor)",
               "relative humidity (indoor)"
            ],
            "sensorModuleType":2,
            "number_of_values":2,
            "campaign_name":"Measurement Campaign Winter 18/19",
            "campaign_description":"Building 3"
         }
      ],
      [
         {
            "date":1583780593,
            "value":21.7609
         },
         {
            "date":1583780294,
            "value":21.8757
         },
         {
            "date":1583779393,
            "value":21.7742
         }
      ],
      [
         {
            "date":1583780593,
            "value":51.0765
         },
         {
            "date":1583780294,
            "value":50.7057
         },
         {
            "date":1583779393,
            "value":48.9708
         }
      ]
   ],
   [
      [
         {
            "node_id":805,
            "units":[
               "W\/m2"
            ],
            "description":[
               "Heat flux"
            ],
            "entities":[
               "heat flux"
            ],
            "sensorModuleType":7,
            "number_of_values":1,
            "campaign_name":"Measurement Campaign Winter 18/19",
            "campaign_description":"Building 3"
         }
      ],
      [
         {
            "date":1583780845,
            "value":23.8
         },
         {
            "date":1583780545,
            "value":27.5
         },
         {
            "date":1583780245,
            "value":28.9
         }
      ]
   ]
]
/************************************************/

/************************************************
/ Configurations
/************************************************/
    include_once($_SERVER['DOCUMENT_ROOT'].'/wsn/config.php');
    include_once($_SERVER['DOCUMENT_ROOT'].'/wsn/php_helper_functions.php');
    ini_set('memory_limit', '-1');
    
/************************************************
// DB connection 
/************************************************/
    // Create connection
    $conn = new mysqli($db_server, $db_username, $db_password, $db_name);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 
    $token = mysqli_real_escape_string($conn, $_GET["token"]);
    
/************************************************
// Check access token for validity
/************************************************/
    // Look for access token in database
    $type = "read";
    $access = check_token($token, $type);
    
    // Abort script if access is not granted.
    if (!$access){
      echo "<div>Invalid Access Token</div>";
      exit();
    } 
    
    // Continue if access token is valid

/************************************************
// Parse GET parameters
/************************************************/

    // Default data
    $number_of_values = 0;
    $entities = ["no data"];
    $units = ["no data"];
    $description = ["no data"];
    $date_start = "";
    $date_end = "";
    $sensorModuleType = -2;

    // GET parameters
    if (!empty($_GET["node_id"])){
        $node_id = mysqli_real_escape_string($conn, $_GET["node_id"]);
        if (strcmp($node_id, "all")!=0){
          $node_id = explode(",", $node_id);
          $node_id = array_map('intval', $node_id);
        }
    }
    if (!empty($_GET["date_start"])){
        $date_start = mysqli_real_escape_string($conn, $_GET["date_start"]);
    }
    if (!empty($_GET["date_end"])){
        $date_end = mysqli_real_escape_string($conn, $_GET["date_end"]);
    }
    if (!empty($_GET["samples"])){
        $samples = mysqli_real_escape_string($conn, $_GET["samples"]);
    }
    if (!empty($_GET["campaign_name"])){
        $campaign_name = mysqli_real_escape_string($conn, $_GET["campaign_name"]);
    }
    if (!empty($_GET["campaign_description"])){
        $campaign_description = mysqli_real_escape_string($conn, $_GET["campaign_description"]);
    }

/************************************************
// Fetch data
/************************************************/

    // Get end date
    if (strcmp($date_end, "")==0){
        if (isset($campaign_name) AND isset($campaign_description)){
            $sql = "SELECT `start_date`, `end_date` FROM `wsn_measurement_campaign` WHERE `campaign_name`='$campaign_name' AND `description`='$campaign_description' ORDER BY `id` DESC LIMIT 1";
            $result = $conn->query($sql);
            if ($result->num_rows > 0){
              $row = $result->fetch_array(MYSQLI_ASSOC);
              $date_start  = $row["start_date"];
              $date_end  = $row["end_date"];
              if (strtotime($date_end) < strtotime($date_start)){
                $date_end = date('Y-m-d H:i:s');            // Overwrite date_end if it before start_date, e.g. default data 00-00-0000 00:00:00
              }
            } else {  // Campaign not found
              $date_end = date('Y-m-d H:i:s');
              $campaign_name = "Campaign_name not found";
              $campaign_description = "Campaign_description not found";
            }
        }
        else {
            $date_end = date('Y-m-d H:i:s');
        }
    }

    // Get list of ids from campaign_name & campagin_description
    if (isset($campaign_name) AND isset($campaign_description) AND is_string($node_id)){
      $sql = "SELECT `node_id_list` FROM `wsn_measurement_campaign` WHERE `campaign_name`='$campaign_name' AND `description`='$campaign_description' ORDER BY `id` DESC LIMIT 1";
      $result = $conn->query($sql);
      if ($result->num_rows > 0){
        $row = $result->fetch_array(MYSQLI_ASSOC);
        $id_list = $row["node_id_list"];
        $id_list = explode(", ", $id_list);
        $node_id = array_map('intval', $id_list);
      } else {
        $campaign_name = "Campaign_name not found";
        $campaign_description = "Campaign_description not found";
        $node_id = array();
      }
    }

    if (!isset($campaign_description)){
      $campaign_name = "";
    }
    if (!isset($campaign_description)){
      $campaign_description = "";
    }

    $myValueArray = array();
    for ($i=0; $i<count($node_id); $i++){
      $myValueArray[$i] = array();
      // Get sensorModuleType and other meta data
      $sql = "SELECT `sensorModuleType` FROM `wsn_input` WHERE `node_id`=$node_id[$i] AND `time`<'$date_end' AND sensorModuleType!=20 ORDER BY `id` DESC LIMIT 1";
      $result = $conn->query($sql);
      if ($result->num_rows > 0){
        $row = $result->fetch_array(MYSQLI_ASSOC);
        $sensorModuleType = (int)$row["sensorModuleType"];
        // Get meta data
        $sql = "SELECT number_of_values, description, unit, entity FROM wsn_sensor_module_type WHERE id=$sensorModuleType ORDER BY id DESC LIMIT 1";
        $result = $conn->query($sql);
        $row = $result->fetch_array(MYSQLI_ASSOC);
        $number_of_values = (int)$row["number_of_values"];
        $entities = explode(', ', $row["entity"]);
        $units = explode(',', $row["unit"]);
        $description = explode(', ', $row["description"]);
     }

      // Get measurement values
      $sql = "SELECT UNIX_TIMESTAMP(time) as date, value0 , value1, value2, value3, value4 FROM wsn_input WHERE node_id=$node_id[$i] AND sensorModuleType=$sensorModuleType";

      if (strcmp($date_start, "")!=0) {
        $sql .= " AND time>'$date_start'";
      }
      if (strcmp($date_end, "")!=0) {
        $sql .= " AND time<'$date_end'";
      }
      $sql .= " ORDER BY id DESC";
      if (isset($samples)){
        $sql .= " LIMIT $samples";
      }
      $result = $conn->query($sql);
      // Add meta data to output array
      $myValueArray[$i][0][]= array("node_id"=>(int)$node_id[$i], "units"=>$units, "description"=>$description, "entities"=>$entities, "sensorModuleType"=>$sensorModuleType, "number_of_values"=>$number_of_values, "campaign_name"=>$campaign_name, "campaign_description"=>$campaign_description);

      // Add measurement data to output array
      if ($result->num_rows > 0) {
          // Read data of each row
          while($row = $result->fetch_array(MYSQLI_ASSOC)) {
              for ($j=0; $j<$number_of_values; $j++ ){
                  $myValueArray[$i][$j+1] [] = array("date"=>(int)$row["date"], "value"=>(float)$row["value$j"]);
              }
          }
      } else {
          $myValueArray[$i][1] [] = array();
      }
    }

/************************************************
// Output json
/************************************************/
  header("Content-Type: application/json;charset=utf-8");
  echo json_encode($myValueArray);

/************************************************
/ Clean up
/************************************************/
// Close db-connection
    $conn->close();
?>