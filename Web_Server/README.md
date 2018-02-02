# Webserver
Two tasks are assigned to the webserver: 1. Inserting the measurement data into the database and 2. Displaying the measured data. All scripts are written in PHP.

## Storing Measured Data
All the files for storing the measured data in the database are in the /gateway2mysql/ directory. The main file is gateway2msysql.php, which is also called by the gateway. The data is attached as an URL-parameter.
Currently, battery warning messages and data from the heat flux sensors need to be treated separately, since these messages do not yet adhere to the standard formatting. 

## Displaying Data
Files for these tasks can be found in the /vis/ directory. The main file is isituprightnow_v3.php. The site shows tables. The left hand side table gives an overview of the gateway activity. The gateway number is clickable and leads to an overview of the sensor nodes. The right hand side table gives an overview of all the sensor nodes of one gateway. The node id is clickable and links to a graph of the respective measured data.
![Graph](/Images/data_graph_example.png)
![Table](/Images/isituprightnow.png)
