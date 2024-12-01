#include <WiFi.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include "camera.h"

#define DHTTYPE DHT22
const int dhtPin = 33;
DHT dht(dhtPin, DHTTYPE);

int motionPort = 32;
int tempPort = 33;
int rainPort = 34;
int motorPort = 35;

const char* ssid1 = "DanieL";
const char* password1 = "zazaza6655";

void initWifi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid1, password1);
  Serial.print("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(String("\nConnected to the WiFi network (") + ssid1 + ")");
  Serial.print("\nStation IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  pinMode(rainPort, INPUT);
  pinMode(motorPort, OUTPUT);
  pinMode(motionPort, INPUT);
  Serial.begin(115200);
  setupCamera();

  initWifi();

  dht.begin();
}

void readTemp() {
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();

  // if (isnan(temp) || isnan(humid)) {
  //   Serial.println("\nFailed to read from DHT sensor!");
  //   return;
  // }

  Serial.print("\nCurrent temp : ");
  Serial.print(temp);
  Serial.print(" °C");
  Serial.print("\nCurrent humidity : ");
  Serial.print(humid);
  Serial.println(" %");
}

void readRain() {
  int rain = analogRead(rainPort);
  Serial.print("\nCurrent rain : ");
  Serial.println(rain);
}

void readMotion() {
  int hasMotion = digitalRead(motionPort);
  Serial.print("\nMotion Sensor : ");
  if (hasMotion == LOW) {
    Serial.println("No Motion");
  } else {
    Serial.println("Motion Detected");
  }
}

void loop() {
  readTemp();
  readRain();
  readMotion();
  Serial.println();
  delay(2000);
}