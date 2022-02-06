void connect_to_wifi() {
  int status = WL_IDLE_STATUS;
  
   if (WiFi.status() == WL_NO_MODULE) {
    #ifdef DEBUGSERIAL
    Serial.println("Communication with WiFi module failed!");
    #endif
    digitalWrite(LED_BUILTIN, HIGH);
    while (true);
   }
    
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    #ifdef DEBUGSERIAL
    Serial.println("Please upgrade the firmware");
    #endif
  }

  while (status != WL_CONNECTED) {
    #ifdef DEBUGSERIAL
    Serial.print("[WiFi] Connecting to: ");
    Serial.println(SSID);
    #endif

    status = WiFi.begin(SSID, PASS);
    delay(10000);
  }
  #ifdef DEBUGSERIAL
  Serial.println("[WiFi] Connected");
  #endif
}

void reconnect_wifi() {
  int status = WL_IDLE_STATUS;
  WiFi.end();
  status = WiFi.begin(SSID, PASS);
   if (WiFi.status() == WL_NO_MODULE) {
    #ifdef DEBUGSERIAL
    Serial.println("Communication with WiFi module failed!");
    #endif
    digitalWrite(LED_BUILTIN, HIGH);
    while (true);
  }
  int i = 0;
  status = WiFi.status();
  while (status != WL_CONNECTED && i < 5) {
    #ifdef DEBUGSERIAL
    Serial.print("[WiFi] Reconnecting to: ");
    Serial.println(SSID);
    #endif
    status = WiFi.begin(SSID, PASS);
    delay(10000);
    i++;
  }

  if (status == WL_CONNECTED) {
    #ifdef DEBUGSERIAL
    Serial.println("[WiFi] Successfully reconnected");
    #endif
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    #ifdef DEBUGSERIAL
    Serial.println("[WiFi] Reconnect attempt unsuccessful");
    #endif
  }
}

void printWifiStatus() {
  #ifdef DEBUGSERIAL
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  #endif
}

void rpi_send() {
  WiFiClient client;
  IPAddress server(192,168,100,254);
  #ifdef DEBUGSERIAL
  Serial.println("Connecting to Raspberry Pi...");
  #endif
  if (client.connect(server, 80)) {
    
    String content = "pass=" + String(SERVER_SECRET) + "&date_time=" + String(rtc1.getYear()) + "-" + String(rtc1.getMonth()) + "-" + String(rtc1.getDay()) + "%20" + String(rtc1.getHours()) + ":" + String(rtc1.getMinutes()) + ":" + String(rtc1.getSeconds()) + "&temp=" + String(t) + "&humidity=" + String(h) + "&pressure=" + String(p);
    
    #ifdef DEBUGSERIAL
    Serial.println("Connected to Raspberry Pi and sending data......");
    #endif
    client.println("POST /upload_data.php HTTP/1.1");
    client.println("Host: 192.168.100.254");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: "); client.println(String(content.length()));
    client.println("Connection: close");
    client.println();
    client.println(content);
  } else {
    #ifdef DEBUGSERIAL
    Serial.println("connection to RPi HTTP server failed");
    #endif
    outage = 1;
    analogWrite(LED_BUILTIN, 50); //turn on the led as an error sign
    delay(5000);
  }
  #ifdef DEBUGSERIAL
  delay(5000);
  while(client.available()) {
    char c = client.read();
    Serial.print(c);  } //print server response to serial
  #endif
  client.stop();
}

void discord_send(String content) {
  const char server2[] = "discord.com";
  const int port2 = 443;
  WiFiSSLClient client2;
  #ifdef DEBUGSERIAL
  Serial.println("Connecting to Discord...");
  #endif
  if (client2.connect(server2, port2)) {
    #ifdef DEBUGSERIAL
    Serial.println("connected to Discord server and sending message...");
    #endif
    client2.println("POST /api/webhooks/818439468100616192/oWiqJRfiZh2o3Fu2eSHDtYBt8r2cyonL3x9ZSn6WxOIIDshvsIJlPbAaGK62bgT3Wvt3 HTTP/1.1");
    client2.println("Host: discord.com");
    client2.println("Content-type: application/json");
    client2.print("Content-Length: "); //délka = 14 + (obsah zprávy)
    client2.println(String(content.length() + 14));
    client2.println();
    client2.print("{\"content\":\"");
    client2.print(content);
    client2.println("\"}");
  } else {
    #ifdef DEBUGSERIAL
    Serial.println("connection to discord failed");
    #endif
    analogWrite(LED_BUILTIN, 50); //turn on the led as an error sign
    delay(5000);
  }
  client2.stop();
}
