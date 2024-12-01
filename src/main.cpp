#include <WiFi.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include "camera.h"

#define DHTTYPE DHT11
const int dhtPin = 33;
DHT dht(dhtPin, DHTTYPE);

int motionPort = 14; // Use a pin from ADC1, e.g., GPIO14
int rainPort = 34;   // GPIO34 is fine for analog input

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
  pinMode(motionPort, INPUT);
  Serial.begin(115200);
  
  // setupCamera(); // Uncomment if you plan to use a camera

  initWifi();

  dht.begin();

  // Give the PIR sensor a few seconds to stabilize
  delay(5000);
}

void readTemp() {
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();

  if (isnan(temp) || isnan(humid)) {
    Serial.println("\nFailed to read from DHT sensor!");
    return;
  }

  Serial.print("Current temp : ");
  Serial.print(temp);
  Serial.print(" °C");
  Serial.print("\nCurrent humidity : ");
  Serial.print(humid);
  Serial.println(" %");
}

void readRain() {
  int rain = analogRead(rainPort);
  Serial.print("Current rain : ");
  Serial.println(rain);
  // Optionally, convert 'rain' to a percentage or meaningful value
}

void readMotion() {
  int motion = digitalRead(motionPort);

  if (motion == LOW) {
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
  delay(2000);  // Delay between readings
}
