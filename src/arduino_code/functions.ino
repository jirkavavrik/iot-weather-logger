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

void reconnect_mqtt() {
  while (!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (mqttclient.connect("arduino")) {
      Serial.println("connected");
      //client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
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
  #ifdef DEBUGSERIAL
  Serial.println("Connecting to Raspberry Pi...");
  #endif
  
  if (!mqttclient.connected()) {
    analogWrite(LED_BUILTIN, 50); //turn on the led as an error sign
    reconnect_mqtt();
  }

    #ifdef DEBUGSERIAL
    Serial.println("Connected to Raspberry Pi and sending data......");
    #endif
  
  char payload[255];
  sprintf(payload,"%s%f%s%f%s%f%s", "{\"temp\":\"",t,"\",\"humidity\":\"",h,"\",\"pressure\":\"",p,"\"}");
  
  #ifdef DEBUGSERIAL
  Serial.println("Publishing message... payload:");
  Serial.println(payload);
  #endif
  mqttclient.publish("meteostanice/data",payload);


  #ifdef DEBUGSERIAL
  delay(5000);
  #endif
  //wificlient.stop();
}
