//#define DEBUGSERIAL
#include <ArduinoLowPower.h>
#include <Adafruit_Sensor.h>  //needed for dht
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

void connect_to_wifi();
void reconnect_wifi();
void printWifiStatus();
void rpi_send();
void discord_send(String);

Adafruit_BMP085 bmp180;
int bmp_correction = 3550; //in Pa
DHT dht(2, DHT22);
RTCZero rtc1;

float h=0,t=0,p=0;
int outage=0;

unsigned long lastConnectionTime = 0L;       // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 120L * 1000L; // delay between updates, in milliseconds
unsigned long lastDiscordConnectionTime = 0L;       // last time you connected to the server, in milliseconds
const unsigned long discordPostingInterval = 3600L * 1000L; // delay between updates, in milliseconds

const char SSID[]     = SECRET_SSID;
const char PASS[]     = SECRET_PASS;

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
  #ifdef DEBUGSERIAL
  //Serial.end();
  #endif
}

void loop() {
  #ifdef DEBUGSERIAL
  //Serial.begin(9600);
  //while (!Serial) {}
  #endif
  if (millis() - lastConnectionTime > postingInterval || lastConnectionTime == 0 || millis() < lastConnectionTime ) {
    if (rtc1.getYear() > 20) { //otherwise it's bullshit and the rtc1 hasn't synchronised
      WiFi.noLowPowerMode();
      delay(2000);//otherwise the DHT doesn't read properly
      h = dht.readHumidity();
      t = dht.readTemperature();
      p = (bmp180.readSealevelPressure(330)/100);
      if(WiFi.status() == WL_CONNECTED){
        rpi_send();
        if (millis() - lastDiscordConnectionTime > discordPostingInterval || lastDiscordConnectionTime == 0 || millis() < lastDiscordConnectionTime ) {
          if(outage) { discord_send("Byl zaznamenán výpadek připojení k WiFi síti, nebo bylo Raspberry Pi nějakou dobu offline - některá data pravděpodobně nebyla nahrána do databáze!"); outage = 0; }
          lastDiscordConnectionTime = millis(); //only for regular reports
        }
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
    } else analogWrite(LED_BUILTIN, 50); //turn on the led as a error sign
  }
  #ifdef DEBUGSERIAL
  //Serial.end();
  #endif

  #ifndef DEBUGSERIAL
  LowPower.idle(1000);
  #endif
}
