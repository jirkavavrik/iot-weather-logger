//#define DEBUGSERIAL
#include <ArduinoLowPower.h>
//#include <Adafruit_Sensor.h>  //needed for dht
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include "functions.h"

Adafruit_BMP085 bmp180;
int bmp_correction = 3550; //in Pa
DHT dht(2, DHT22);
RTCZero rtc1;

float h=0,t=0,p=0;
int outage=0,sd_problem=0;

unsigned long lastConnectionTime = 0L;       // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 120L * 1000L; // delay between updates, in milliseconds
unsigned long lastDiscordConnectionTime = 0L;       // last time you connected to the server, in milliseconds
const unsigned long discordPostingInterval = 3600L * 1000L; // delay between updates, in milliseconds

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A1, INPUT);
  digitalWrite(LED_BUILTIN, HIGH);
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
        if (millis() - lastDiscordConnectionTime > discordPostingInterval || lastDiscordConnectionTime == 0 || millis() < lastDiscordConnectionTime ) {
          String discord_message = String( "Pravidelné hlášení: teplota = " + String(t) + " C, vlhkost = " + String(h) + " %, tlak = " + String(p) + " hPa" );
          discord_send(discord_message);
          if(outage) { discord_send("Byl zaznamenán výpadek připojení k WiFi síti, nebo bylo Raspberry Pi nějakou dobu offline - některá data pravděpodobně nebyla nahrána do databáze!"); outage = 0; }
          if(backupVoltage < 3500){ discord_send("POZOR - vybitý akumulátor, ale funkční WiFi připojení - pravděpodobný problém s napájením. Napětí (mV): " + String(backupVoltage));  }
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
  Serial.end();
  #endif

  #ifndef DEBUGSERIAL
  LowPower.idle(1000);
  #endif
}
