<?php
require_once "../config.php";
include '../functions.php';
?>


<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<title>IoT meteostanice</title>
        <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.1/css/all.css">
		<style>
table, th, td {
	border: 4px solid black;
}

table {
	width: 75%;
	border-collapse: collapse;
	border-spacing: 5;
	text-align: center;
	margin-left: auto;
	margin-right: auto;
	margin-top: 20px;
	margin-bottom: 20px;
}

</style>
	</head>
	<body class="loggedin">
    <nav class="navtop">
    	<div>
    		<h1>IoT meteostanice</h1>
            <a href="/"><i class="fas fa-home"></i>Domovská stránka</a>
            <a href="../phpmyadmin"><i class="fas fa-database"></i>PMA</a>
    		</div>
    </nav>





<div class="content">
			<h2>Aktuální meteo údaje</h2>
<?php
require_once "../config.php";

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

        while($row = mysqli_fetch_array($result)){
			$date=date_create($row['date_time'],timezone_open("UTC"));
			$date->setTimezone(timezone_open("Europe/Prague"));
			
			echo "<table cellspacing=\"5\">";
            echo "<tr>";
            echo "<th>Datum a čas</th>";
            echo "</tr>";
			echo "<tr>";
            echo "<td>" . date_format($date,"d. m. Y, H:i:s T") . "</td>";
            echo "</tr>";
			echo "</table>";
			
			echo "<table cellspacing=\"5\">";
            echo "<tr>";
            echo "<th>Teplota</th>";
            echo "</tr>";
			echo "<tr>";
            echo "<td>" . $row['temp'] . " °C</td>";
            echo "</tr>";
			echo "</table>";
			
			echo "<table cellspacing=\"5\">";
            echo "<tr>";
            echo "<th>Relativní vlhkost</th>";
            echo "</tr>";
			echo "<tr>";
            echo "<td>" . $row['humidity'] . " %</td>";
            echo "</tr>";
			echo "</table>";
			
			echo "<table cellspacing=\"5\">";
            echo "<tr>";
            echo "<th>Tlak (přepočtený na hladinu moře)</th>";
            echo "</tr>";
			echo "<tr>";
            echo "<td>" . $row['pressure'] . " hPa</td>";
            echo "</tr>";
			echo "</table>";
			
        }
        
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