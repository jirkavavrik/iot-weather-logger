<?php
require_once "config.php";

/* Attempt to connect to MySQL database */
$link = mysqli_connect(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_NAME);

// Check connection
if($link === false){
    die("ERROR: Could not connect. " . mysqli_connect_error());
}

//escapuje nedovolené znaky ze superglob. proměnné
$pass = mysqli_real_escape_string($link, $_POST["pass"]);
$date_time = mysqli_real_escape_string($link, $_POST["date_time"]);
$temp = mysqli_real_escape_string($link, $_POST["temp"]);
$humidity = mysqli_real_escape_string($link, $_POST["humidity"]);
$pressure = mysqli_real_escape_string($link, $_POST["pressure"]);

if( $temp == "nan") {
	$temp = "NULL";
} 

if( $humidity == "nan") {
	$humidity = "NULL";
} 

if( $pressure == "nan") {
	$pressure = "NULL";
} 

// Attempt select query execution
if(password_verify($pass, $pass_hash)) { 
    $query = sprintf("INSERT INTO log(date_time, temp, humidity, pressure) VALUES('%s', %s, %s, %s);", $date_time, $temp, $humidity, $pressure);
} else {
    die("not allowed");
}
    

if($result = mysqli_query($link, $query)){
    echo "success";
} else {
    echo "ERROR: Could not execute $sql. " . mysqli_error($link);
}

// Close connection
mysqli_close($link);
?>