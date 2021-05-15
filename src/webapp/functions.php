<?php

function template_header($title) {
echo <<<EOT
<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<title>$title</title>
		<link href="style.css" rel="stylesheet" type="text/css">
        <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.1/css/all.css">
	</head>
	<body class="loggedin">
    <nav class="navtop">
    	<div>
    		<h1>IoT meteostanice</h1>
            <a href="/"><i class="fas fa-home"></i>Domovská stránka</a>
            <a href="dump_table.php"><i class="fas fa-database"></i>Výpis dat</a>
    		</div>
    </nav>
EOT;
}


function template_footer() {
echo <<<EOT
    </body>
</html>
EOT;
}
?>