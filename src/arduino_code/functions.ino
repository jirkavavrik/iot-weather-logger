void inline heater_status_check() {
  if (!sht30.isHeaterEnabled()) {
    return;
  }
  if(millis() - heaterTurnedOnTime > heaterOnInterval || heaterTurnedOnTime == 0 || millis() < heaterTurnedOnTime) {
    sht30.heater(false);
    #ifdef DEBUGSERIAL
    Serial.println("Heater OFF - performed by function heater_status_check(); ");
    #endif
  }
}

int connect_to_wifi() {
  int i = 0;
  int status = WL_IDLE_STATUS;
  
   if (WiFi.status() == WL_NO_MODULE) {
    #ifdef DEBUGSERIAL
    Serial.println("Communication with WiFi module failed!");
    #endif
    digitalWrite(LED_BUILTIN, HIGH);
    return 1;//while (true);
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
    heater_status_check();
    i++;
    if(i>4) {
      #ifdef DEBUGSERIAL
      Serial.println("Wi-Fi (re)connection failed after reaching maximum number of attempts, exiting from connect_to_wifi();");
      #endif
      return 1;
    }
  }
  #ifdef DEBUGSERIAL
  Serial.println("[WiFi] Connected");
  #endif
  return 0;
}

int reconnect_wifi() {
  int i = 0;
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
  status = WiFi.status();
  while (status != WL_CONNECTED) {
    #ifdef DEBUGSERIAL
    Serial.print("[WiFi] Reconnecting to: ");
    Serial.println(SSID);
    #endif
    status = WiFi.begin(SSID, PASS);
    delay(10000);
    heater_status_check();
        i++;
    if(i>4) {
      #ifdef DEBUGSERIAL
      Serial.println("Wi-Fi (re)connection failed after reaching maximum number of attempts, exiting from reconnect_wifi();");
      #endif
      return 1;
    }
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

int reconnect_mqtt() {
  int i = 0;
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
    heater_status_check();
    i++;
    if(i>4) {
      #ifdef DEBUGSERIAL
      Serial.println("MQTT (re)connection failed after reaching maximum number of attempts, exiting from reconnect_mqtt();");
      #endif
      return 1;
    }
  }
  return 0;
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
    if(reconnect_mqtt() != 0) {
      return;
    }
  }

  #ifdef DEBUGSERIAL
  Serial.println("Connected to Raspberry Pi and sending data......");
  #endif
  
  char payload[8];
  
  sprintf(payload,"%.2f", t);
  #ifdef DEBUGSERIAL
  Serial.println("Publishing message... payload:");
  Serial.println(payload);
  #endif
  mqttclient.publish("meteostanice/teplota", payload, true);
  
  sprintf(payload,"%.2f", h);
  #ifdef DEBUGSERIAL
  Serial.println("Publishing message... payload:");
  Serial.println(payload);
  #endif
  mqttclient.publish("meteostanice/vlhkost", payload, true);
  
  sprintf(payload,"%.2f", p);
  #ifdef DEBUGSERIAL
  Serial.println("Publishing message... payload:");
  Serial.println(payload);
  #endif
  mqttclient.publish("meteostanice/tlak", payload, true);


  #ifdef DEBUGSERIAL
  delay(5000);
  #endif
  //wificlient.stop();
}
