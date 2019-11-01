<!------------------------------------------------------------------------------
 graph_db_v1 
 This script takes the node id and displays time series data.
 The time series data is fetched by det_data_v2.php.
 Helper functions are in php_helper_functions.php.
 To plot the time series data, the metrics graphics framework is used.
 Author: Mario Frei (2018)
 ------------------------------------------------------------------------------>
<html lang='en'>
<head>
    <meta content='text/html; charset=utf-8' http-equiv='Content-Type'>
    <title>MetricsGraphics.js - Examples</title>

    <link href='./dist/metricsgraphics.css' rel='stylesheet' type='text/css'>
    <link href='./css/metricsgraphics-demo.css' rel='stylesheet' type='text/css' id='light'>

    <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/3.1.0/jquery.min.js'></script>
    <script src="https://d3js.org/d3.v4.min.js"></script>
    <script src='./src/js/MG.js'></script>
    <script src='./src/js/misc/utility.js'></script>
    <script src='./src/js/common/data_graphic.js'></script>
    <script src='./src/js/common/hooks.js'></script>
    <script src='./src/js/common/register.js'></script>
    <script src='./src/js/common/bootstrap_tooltip_popover.js'></script>
    <script src='./src/js/common/scales.js'></script>
    <script src='./src/js/common/chart_title.js'></script>
    <script src='./src/js/common/y_axis.js'></script>
    <script src='./src/js/common/x_axis.js'></script>
    <script src='./src/js/common/scales.js'></script>
    <script src='./src/js/common/init.js'></script>
    <script src='./src/js/common/markers.js'></script>
    <script src='./src/js/common/rollover.js'></script>
    <script src='./src/js/common/window_listeners.js'></script>
    <script src='./src/js/layout/bootstrap_dropdown.js'></script>
    <script src='./src/js/layout/button.js'></script>
    <script src='./src/js/charts/line.js'></script>
    <script src='./src/js/charts/histogram.js'></script>
    <script src='./src/js/charts/point.js'></script>
    <script src='./src/js/charts/bar.js'></script>
    <script src='./src/js/charts/table.js'></script>
    <script src='./src/js/charts/missing.js'></script>
    <script src='./src/js/misc/process.js'></script>
    <script src='./src/js/misc/smoothers.js'></script>
    <script src='./src/js/misc/formatters.js'></script>
    <script src='./src/js/misc/transitions.js'></script>
    <script src='./src/js/misc/error.js'></script>
    <script src='js/addons/mg_line_brushing.js'></script>
    <link rel='stylesheet' href='css/addons/mg_line_brushing.css' />
    <?include_once($_SERVER['DOCUMENT_ROOT'].'/wsn/php_helper_functions.php');?>
</head>

<!------------------------------------------------------------------------------
Setup some divs that can be manipulated by javascript
 ------------------------------------------------------------------------------>
<body>
<div align="center">                
    <div id='myGraph'></div>     
    <div id='myLegend'></div>                 
</div>


<!------------------------------------------------------------------------------
The actual plotting of the data happens in the following javascript lines.
 ------------------------------------------------------------------------------>
<script>
MG._hooks = {};

d3.json('./get_data_v2_heartbeat.php?node_id=<?echo $_GET["node_id"];?>', function(data) {
    for (var i = 0; i < data.length; i++) {
        data[i] = MG.convert.date(data[i], 'date', "%Y-%m-%d %H:%M:%S" );
    }

    MG.data_graphic({
        title: "Node <?echo $_GET["node_id"];?>",
        data: data,
        width: 800,
        height: 400,
        right: 40,
        interpolate: d3.curveLinear,
        target: '#myGraph',
        missing_is_hidden: true,
        legend: ['<?echo get_entity($_GET["node_id"],0);?>','<?echo get_entity($_GET["node_id"],1);?>','<?echo get_entity($_GET["node_id"],2);?>'],
        legend_target: '#myLegend',
        min_y_from_data: true,
        //min_y: 20,
        //max_y: 35,
        //y_label: 'My y-Label',
        aggregate_rollover: true
    });
});

</script>

    <script src='js/main.js'></script>
</body>
</html>
