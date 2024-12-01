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

  Serial.println();
  Serial.print("\nStation IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  pinMode(rainPort, INPUT);
  pinMode(tempPort, INPUT);
  pinMode(motorPort, OUTPUT);
  pinMode(motionPort, INPUT);
  Serial.begin(115200);

  initWifi();

  dht.begin();

  // dht.setup(tempPort);

}

void readTemp() {
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();
  Serial.print("\nCurrent temp : ");
  Serial.print(temp);
  Serial.print("\nCurrent humidity : ");
  Serial.print(humid);
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
    Serial.print("Motion Deteced");
  } else {
    Serial.print("No Motion");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  readTemp();
  readRain();
  readMotion();

  Serial.print("\n");
  delay(1000);


}

