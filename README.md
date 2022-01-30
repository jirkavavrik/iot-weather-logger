# iot-weather-logger

IoT weather logger is an Arduino-based weather station that sends data about temperature, humidity and air pressure periodically to a webserver running a simple interface to watch these data.

![Photo 1 of the weather logger](doc/photo1.jpg)
![Photo 1 of the weather logger](doc/photo2.jpg)

## Ignored files should look like this:

### src/arduino_code/arduino_secrets.h
```
#pragma once
#define SECRET_SSID "your ssid"
#define SECRET_PASS "your password"

```

### src/webapp/config.php
```
<?php
define('DB_SERVER', 'localhost');
define('DB_USERNAME', 'user');
define('DB_PASSWORD', 'your password');
define('DB_NAME', 'your db name');

$pass_hash = '$2y$10$TJ9NucYXdwnL8ARpohL9nuE/C6o5XphgRZSSaguxIvM0wETDJrdtu';
?>
```