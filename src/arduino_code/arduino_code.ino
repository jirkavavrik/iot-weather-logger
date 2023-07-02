#include <PubSubClient.h>
//#define DEBUGSERIAL
#include <ArduinoLowPower.h>
#include "Adafruit_SHT31.h"
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

int connect_to_wifi();
int reconnect_wifi();
void printWifiStatus();
void rpi_send();
int reconnect_mqtt();
void heater_status_check();

Adafruit_BMP085 bmp180;
int bmp_correction = 3550; //in Pa
Adafruit_SHT31 sht30 = Adafruit_SHT31(); //works also for sht30

float h=0,t=0,p=0;
int outage=0;

unsigned long lastConnectionTime = 0L;       // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 300L * 1000L; // delay between updates, in milliseconds

unsigned long heaterTurnedOnTime = 0L;       // last time heater was on, in milliseconds
const unsigned long heaterOnInterval = 75L * 1000L; //max interval for heater to be on, in milliseconds

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

  sht30.begin(0x44);
  bmp180.begin();
  
  #ifdef DEBUGSERIAL
  Serial.print("Heater Enabled State: ");
  if (sht30.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
  #endif

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
    delay(2000);
    h = sht30.readHumidity();
    t = sht30.readTemperature();
    p = (bmp180.readSealevelPressure(330)/100);

    if(h >= 90.0f) {
      sht30.heater(true);
      heaterTurnedOnTime = millis();
      #ifdef DEBUGSERIAL
      Serial.println("Heater ON");
      #endif
    }
    
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

    if(sht30.isHeaterEnabled()) {
      while(millis() - heaterTurnedOnTime < heaterOnInterval && heaterTurnedOnTime != 0 && millis() > heaterTurnedOnTime) {
        delay(1000);
      }
      sht30.heater(false);
      #ifdef DEBUGSERIAL
      Serial.println("Heater OFF");
      #endif
    }

  }
  #ifndef DEBUGSERIAL
  LowPower.idle(1000);
  #endif
}
