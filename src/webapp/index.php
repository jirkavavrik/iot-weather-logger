<?php
require_once "config.php";
include 'functions.php';

/* Attempt to connect to MySQL database */
$link = mysqli_connect(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_NAME);

// Check connection
if($link === false){
    die("ERROR: Could not connect. " . mysqli_connect_error());
}

$len = mysqli_real_escape_string($link, $_GET["len"]);

if($len == 1){
$sql = "SELECT * FROM log WHERE HOUR(TIMEDIFF((SELECT CURRENT_TIMESTAMP()), date_time)) <= 24 ORDER BY date_time DESC";
} elseif ($len == 2){
$sql = "SELECT * FROM log WHERE HOUR(TIMEDIFF((SELECT CURRENT_TIMESTAMP()), date_time)) <= 168 ORDER BY date_time DESC";
} elseif ($len == 3){
$sql = "SELECT * FROM log WHERE DATEDIFF((SELECT CURRENT_TIMESTAMP()), date_time) <= 30 ORDER BY date_time DESC";
} elseif ($len == 4){
$sql = "SELECT * FROM log WHERE DATEDIFF((SELECT CURRENT_TIMESTAMP()), date_time) <= 365 ORDER BY date_time DESC";
} elseif ($len == 5){
$sql = "SELECT * FROM log ORDER BY date_time DESC";
}
else {
	$sql = "SELECT * FROM log WHERE HOUR(TIMEDIFF((SELECT CURRENT_TIMESTAMP()), date_time)) <= 24 ORDER BY date_time DESC";
}

//$sql = "SELECT * FROM log ORDER BY date_time DESC";
if($result = mysqli_query($link, $sql)){

	$tempDataPoints = array();
	$humidityDataPoints = array();
	$pressureDataPoints = array();

	while($row = mysqli_fetch_array($result)){
		array_push($tempDataPoints, array("x" => strtotime($row['date_time'])*1000+3600000, "y" => $row['temp']));
		array_push($humidityDataPoints, array("x" => strtotime($row['date_time'])*1000+3600000, "y" => $row['humidity']));
		array_push($pressureDataPoints, array("x" => strtotime($row['date_time'])*1000+3600000, "y" => $row['pressure']));
	}

//echo json_encode($dataPoints, JSON_NUMERIC_CHECK);

} else{
    echo "ERROR: Could not execute $sql. " . mysqli_error($link);
}

// Close connection
mysqli_close($link);
?>
<?=template_header('IoT meteostanice - domovská stránka')?>
<script>
window.onload = function () {
 
var chart = new CanvasJS.Chart("chartContainer", {
	animationEnabled: true,
	zoomEnabled: true,
	title:{
		text: "Průběh teploty, vlhkosti a tlaku v čase"
	},
	axisX:{
		title: "čas",
		gridDashType: "dot",
		gridThickness: 1
	},
	axisY: [{
		title: "[°C]",
		titleFontColor: "#4F81BC",
		lineColor: "#4F81BC",
		labelFontColor: "#4F81BC",
		tickColor: "#4F81BC"
	}, {
		title: "[%]",
		titleFontColor: "#C0504E",
		lineColor: "#C0504E",
		labelFontColor: "#C0504E",
		tickColor: "#C0504E"
	}],
	axisY2: {
		title: "[hPa]",
		titleFontColor: "#9BBB58",
		lineColor: "#9BBB58",
		labelFontColor: "#9BBB58",
		tickColor: "#9BBB58"
	},
	legend:{
		cursor: "pointer",
		dockInsidePlotArea: true,
		itemclick: toggleDataSeries
	},
	data: [{
		type: "line",
		xValueType: "dateTime",
		name: "Teplota",
		markerSize: 0,
		xValueFormatString: "DD. MM. YYYY, HH:mm:ss",
		toolTipContent: "{x}<br>{name}: {y} °C",
		showInLegend: true,
		dataPoints: <?php echo json_encode($tempDataPoints, JSON_NUMERIC_CHECK); ?>
	},{
		type: "line",
		xValueType: "dateTime",
		axisYIndex: 1,
		//axisYType: "secondary",
		name: "Relativní vlhkost",
		markerSize: 0,
		xValueFormatString: "DD. MM. YYYY, HH:mm:ss",
		toolTipContent: "{x}<br>{name}: {y} %",
		showInLegend: true,
		dataPoints: <?php echo json_encode($humidityDataPoints, JSON_NUMERIC_CHECK); ?>
	},{
		type: "line",
		xValueType: "dateTime",
		//axisYIndex: 2,
		axisYType: "secondary",
		name: "Atmosférický tlak přepočtený na hladinu moře",
		markerSize: 0,
		xValueFormatString: "DD. MM. YYYY, HH:mm:ss",
		toolTipContent: "{x}<br>{name}: {y} hPa",
		showInLegend: true,
		dataPoints: <?php echo json_encode($pressureDataPoints, JSON_NUMERIC_CHECK); ?>
	}]
});
chart.render();
 
function toggleDataSeries(e){
	if (typeof(e.dataSeries.visible) === "undefined" || e.dataSeries.visible) {
		e.dataSeries.visible = false;
	}
	else{
		e.dataSeries.visible = true;
	}
	chart.render();
}
 
}
</script>

<style>
table, th, td {
	border: 4px solid black;
}

table {
	width: 100%;
	border-collapse: collapse;
	border-spacing: 5;
	text-align: center;
}

</style>



<div class="content">
			<h2>Aktuální meteo údaje</h2>
<?php
require_once "config.php";

/* Attempt to connect to MySQL database */
$link = mysqli_connect(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_NAME);

// Check connection
if($link === false){
    die("ERROR: Could not connect. " . mysqli_connect_error());
}

// Attempt select query execution
$sql = "SELECT * FROM log WHERE date_time = (SELECT MAX(date_time) FROM log);";
if($result = mysqli_query($link, $sql)){
    if(mysqli_num_rows($result) > 0){
        echo "<table cellspacing=\"5\">";
            echo "<tr>";
                echo "<th>Datum a čas</th>";
                echo "<th>Teplota</th>";
                echo "<th>Relativní vlhkost</th>";
                echo "<th>Tlak (přepočtený na hladinu moře)</th>";
            echo "</tr>";
        while($row = mysqli_fetch_array($result)){
			$date=date_create($row['date_time'],timezone_open("UTC"));
			$date->setTimezone(timezone_open("Europe/Prague"));
            echo "<tr>";
                echo "<td>" . date_format($date,"d. m. Y, H:i:s T") . "</td>";
                echo "<td>" . $row['temp'] . " °C</td>";
                echo "<td>" . $row['humidity'] . " %</td>";
                echo "<td>" . $row['pressure'] . " hPa</td>";
            echo "</tr>";
        }
        echo "</table>";
        // Free result set
        mysqli_free_result($result);
    } else{
        echo "No records matching your query were found.";
    }
} else{
    echo "ERROR: Could not execute $sql. " . mysqli_error($link);
}

// Close connection
mysqli_close($link);
?>
</div>
<div class="content"><h2>Historická data</h2>
			<a href="?len=1"><button>poslední den (výchozí)</button></a>
			<a href="?len=2"><button>poslední týden</button></a>
			<a href="?len=3"><button>poslední měsíc</button></a>
			<a href="?len=4"><button>poslední rok</button></a>
			<a href="?len=5"><button>vše</button></a>
            <div id="chartContainer" style="height: 370px; width: 100%; margin:0; padding:0;"></div>
            <script src="https://canvasjs.com/assets/script/canvasjs.min.js"></script>
		</div>
	<a href="m/index.php">Mobilní verze bez grafu</a>	
	
<?=template_footer()?>