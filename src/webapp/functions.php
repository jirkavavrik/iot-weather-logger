<?php

function template_header($title) {
echo <<<EOT
<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<title>$title</title>
		<link href="style.css" rel="stylesheet" type="text/css">
        <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.1/css/all.css">
	</head>
	<body class="loggedin">
    <nav class="navtop">
    	<div>
    		<h1>IoT meteostanice</h1>
            <a href="/"><i class="fas fa-home"></i>Domovská stránka</a>
            <a href="adminer.php"><i class="fas fa-database"></i>Adminer</a>
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