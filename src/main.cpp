#include <WiFi.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>

#define DHTTYPE DHT22
const int dhtPin = 33;
DHT dht(dhtPin, DHTTYPE);

int motionPort = 32;
int tempPort = 33;
int rainPort = 34;
int motorPort = 35;

const char* ssid1 = "Chega";
const char* password1 = "qazplmvgty";

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

  initWifi();

  dht.begin();
}

void readTemp() {
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();

  if (isnan(temp) || isnan(humid)) {
    Serial.println("\nFailed to read from DHT sensor!");
    return;
  }

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
  bool hasMotion = digitalRead(motionPort);
  Serial.print("\nMotion Sensor : ");
  if (hasMotion) {
    Serial.println("Motion Detected");
  } else {
    Serial.println("No Motion");
  }
}

void loop() {
  readTemp();
  readRain();
  readMotion();
  Serial.println();
  delay(2000);
}