#include <ArduinoLowPower.h>
#include <SD.h>
#include "arduino_secrets.h"
#include <Adafruit_Sensor.h>  //needed for dht
#include <DHT.h>
#include "Wire.h"
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <RTCZero.h>
//#define DEBUGSERIAL

const char SSID[]     = SECRET_SSID;
const char PASS[]     = SECRET_PASS;
float h; float t; float p;
int outage = 0; int sd_problem = 0;

RTCZero rtc1;
Adafruit_BMP085 bmp180;
int bmp_correction = 3550; //in Pa
DHT dht(7, DHT22);

int status = WL_IDLE_STATUS;

WiFiClient client;
IPAddress server(192,168,100,254);

//DISCORD SETTINGS
const char server2[] = "discord.com";
const int port2 = 443;
WiFiSSLClient client2;

unsigned long lastConnectionTime = 0L;       // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 120L * 1000L; // delay between updates, in milliseconds
unsigned long lastDiscordConnectionTime = 0L;       // last time you connected to the server, in milliseconds
const unsigned long discordPostingInterval = 3600L * 1000L; // delay between updates, in milliseconds

void connect_to_wifi() {
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

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A1, INPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Wire.begin();
  dht.begin();
  bmp180.begin();
  SD.begin(4);

  connect_to_wifi();
  printWifiStatus();

  rtc1.begin();
  unsigned long epoch;
  int numberOfTries = 0, maxTries = 6;
  do {
    epoch = WiFi.getTime();
    numberOfTries++;
  } while ((epoch == 0) && (numberOfTries < maxTries));
  if (numberOfTries == maxTries) {
    #ifdef DEBUGSERIAL
    Serial.print("NTP unreachable!!");
    #endif
    while (1);
  } else {
    #ifdef DEBUGSERIAL
    Serial.print("Epoch received: ");
    Serial.println(epoch);
    Serial.println();
    #endif
    rtc1.setEpoch(epoch);
  }
  digitalWrite(LED_BUILTIN, LOW);
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
  if (client.connect(server, 80)) {
    #ifdef DEBUGSERIAL
    Serial.println("Connected to Raspberry Pi and sending data......");
    #endif
    client.print("GET /upload_data.php?pass=abc123&date_time=");
    client.print(rtc1.getYear());
    client.print("-");
    client.print(rtc1.getMonth());
    client.print("-");
    client.print(rtc1.getDay());
    client.print("%20");
    client.print(rtc1.getHours());
    client.print(":");
    client.print(rtc1.getMinutes());
    client.print(":");
    client.print(rtc1.getSeconds());
    client.print("&temp=");
    client.print(t);
    client.print("&humidity=");
    client.print(h);
    client.print("&pressure=");
    client.print(p);
    client.println(" HTTP/1.1");
    client.println("Host: 192.168.100.254");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } else {
    #ifdef DEBUGSERIAL
    Serial.println("connection to RPi HTTP server failed");
    #endif
    outage = 1;
    analogWrite(LED_BUILTIN, 50); //turn on the led as an error sign
    delay(5000);
  }
  client.stop();
}

void discord_send(String content) {
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

void sd_write() {
  #ifdef DEBUGSERIAL
  Serial.println("Opening file on SD...");
  #endif
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    #ifdef DEBUGSERIAL
    Serial.println("file opened, writing to the file...");
    #endif
    dataFile.print(rtc1.getYear(), DEC);
    dataFile.print(".");
    dataFile.print(rtc1.getMonth(), DEC);
    dataFile.print(".");
    dataFile.print(rtc1.getDay(), DEC);
    dataFile.print(" ");
    dataFile.print(rtc1.getHours(), DEC);
    dataFile.print(":");
    dataFile.print(rtc1.getMinutes(), DEC);
    dataFile.print(":");
    dataFile.print(rtc1.getSeconds(), DEC);
    
    dataFile.print(",");

    dataFile.print("\"");
    dataFile.print(t);
    dataFile.print("\"");

    dataFile.print(",");
    
    dataFile.print("\"");
    dataFile.print(h);
    dataFile.print("\"");

    dataFile.print(",");
    
    dataFile.print("\"");
    dataFile.print((int)p);
    dataFile.print("\"");

    dataFile.println();
    dataFile.close();
  } else {
    #ifdef DEBUGSERIAL
    Serial.println("could not open file on SD!");
    #endif
    sd_problem = 1;
    analogWrite(LED_BUILTIN, 50); //turn on the led as an error sign
    delay(5000);
  }
}

void loop() {
  #ifdef DEBUGSERIAL
  Serial.begin(9600);
  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  #endif
  if (millis() - lastConnectionTime > postingInterval || lastConnectionTime == 0 || millis() < lastConnectionTime ) {
    if (rtc1.getYear() > 20) { //otherwise it's bullshit and the rtc1 hasn't synchronised
      WiFi.noLowPowerMode();
      delay(2000);//otherwise the DHT doesn't read properly
      h = dht.readHumidity();
      t = dht.readTemperature();
      p = (bmp180.readSealevelPressure(330)/100);
      int backupVoltage = (analogRead(A1)) * 16113 / 1000;
      if(WiFi.status() == WL_CONNECTED){
        rpi_send();
        sd_write();
        if (millis() - lastDiscordConnectionTime > discordPostingInterval || lastDiscordConnectionTime == 0 || millis() < lastDiscordConnectionTime ) {
          String discord_message = String( "Pravidelné hlášení: teplota = " + String(t) + " C, vlhkost = " + String(h) + " %, tlak = " + String(p) + " hPa" );
          discord_send(discord_message);
          if(outage) { discord_send("Byl zaznamenán výpadek připojení k WiFi síti, nebo bylo Raspberry Pi nějakou dobu offline - některá data pravděpodobně chybí v sql databázi!"); outage = 0; }
          if(sd_problem) { discord_send("Byl zaznamenán problém se zápisem na SD kartu, některá data pravděpodobně nejsou zálohována!"); sd_problem = 0; }
          if(backupVoltage < 3500){ discord_send("POZOR - vybitý akumulátor, ale funkční WiFi připojení - pravděpodobný problém s napájením. Napětí (mV): " + String(backupVoltage));  }
          lastDiscordConnectionTime = millis(); //only for regular reports
        }
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
        outage = 1;
        sd_write(); 
        reconnect_wifi();
      }
      lastConnectionTime = millis();
      WiFi.lowPowerMode();
      //flash led as a sign of a success
      analogWrite(LED_BUILTIN, 255);
      delay(200);
      analogWrite(LED_BUILTIN, 0);
    } else analogWrite(LED_BUILTIN, 50); //turn on the led as a error sign
  }
  #ifdef DEBUGSERIAL
  Serial.end();
  #endif

  #ifndef DEBUGSERIAL
  LowPower.idle(1000);
  #endif
}
