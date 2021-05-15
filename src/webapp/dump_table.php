<style>
table, th, td {
	border: 1px solid black;
}
table {
	width: 100%;
	border-collapse: collapse;
	text-align: center;
}
</style>

<?php
require_once "config.php";
include 'functions.php';?>



<?=template_header('IoT meteostanice - výpis dat')?>



<div class="content">
<h2>Výpis dat</h2>
<?php
/* Attempt to connect to MySQL database */
$link = mysqli_connect(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_NAME);

// Check connection
if($link === false){
    die("ERROR: Could not connect. " . mysqli_connect_error());
}

// Attempt select query execution
$sql = "SELECT * FROM log ORDER BY date_time DESC;";
if($result = mysqli_query($link, $sql)){
    if(mysqli_num_rows($result) > 0){
        echo "<table>";
            echo "<tr>";
                echo "<th>date_time</th>";
                echo "<th>temp</th>";
                echo "<th>humidity</th>";
                echo "<th>pressure</th>";
            echo "</tr>";
        while($row = mysqli_fetch_array($result)){
            echo "<tr>";
                echo "<td>" . $row['date_time'] . "</td>";
                echo "<td>" . $row['temp'] . "</td>";
                echo "<td>" . $row['humidity'] . "</td>";
                echo "<td>" . $row['pressure'] . "</td>";
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
?></div>
<?=template_footer()?>