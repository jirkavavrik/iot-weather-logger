#include <PubSubClient.h>
//#define DEBUGSERIAL
#include <ArduinoLowPower.h>
#include <Adafruit_Sensor.h>  //needed for dht
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

void connect_to_wifi();
void reconnect_wifi();
void printWifiStatus();
void rpi_send();
void reconnect_mqtt();

Adafruit_BMP085 bmp180;
int bmp_correction = 3550; //in Pa
DHT dht(2, DHT22);

float h=0,t=0,p=0;
int outage=0;

unsigned long lastConnectionTime = 0L;       // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 150L * 1000L; // delay between updates, in milliseconds

const char SSID[]     = SECRET_SSID;
const char PASS[]     = SECRET_PASS;

WiFiClient wificlient;
PubSubClient mqttclient(wificlient);
IPAddress server(192,168,100,254);
//const char server[] = "raspberrypi1.local";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  #ifdef DEBUGSERIAL
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("Starting.......");
  #endif

  Wire.begin();
  dht.begin();
  bmp180.begin();

  connect_to_wifi();
  printWifiStatus();

  unsigned long epoch;
  int numberOfTries = 0, maxTries = 6;

  digitalWrite(LED_BUILTIN, LOW);
  #ifdef DEBUGSERIAL
  //Serial.end();
  #endif

  mqttclient.setServer(server, 1883);
  //mqttclient.setCallback(callback);
}

void loop() {
  mqttclient.loop(); //dokumentace: This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
  
  if (millis() - lastConnectionTime > postingInterval || lastConnectionTime == 0 || millis() < lastConnectionTime ) {
    WiFi.noLowPowerMode();
    delay(2000);//otherwise the DHT doesn't read properly
    h = dht.readHumidity();
    t = dht.readTemperature();
    p = (bmp180.readSealevelPressure(330)/100);
    if(WiFi.status() == WL_CONNECTED){
      rpi_send();
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
      outage = 1;
      reconnect_wifi();
    }
    lastConnectionTime = millis();
    WiFi.lowPowerMode();
    //flash led as a sign of a success
    analogWrite(LED_BUILTIN, 255);
    delay(200);
    analogWrite(LED_BUILTIN, 0);
  }
  #ifndef DEBUGSERIAL
  LowPower.idle(1000);
  #endif
}
