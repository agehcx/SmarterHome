#include <WiFi.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include "camera.h"
#include <PubSubClient.h> // ไลบรารีสำหรับ MQTT

#define DHTTYPE DHT11
const int dhtPin = 33;
DHT dht(dhtPin, DHTTYPE);

int motionPort = 14; // ใช้ GPIO14 สำหรับ Motion Sensor
int rainPort = 34;   // ใช้ GPIO34 สำหรับ Rain Sensor

const char* ssid1 = "Chega";
const char* password1 = "qazplmvgty";

// MQTT Broker Settings
const char* mqtt_server = "broker.hivemq.com"; // หรือใช้ IP ของ Broker
const char* mqtt_topic_temp = "sensor/temp";
const char* mqtt_topic_humid = "sensor/humidity";
const char* mqtt_topic_rain = "sensor/rain";
const char* mqtt_topic_motion = "sensor/motion";

WiFiClient espClient;
PubSubClient client(espClient);

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

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) { // ใช้ชื่ออุปกรณ์เพื่อระบุ Client
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(rainPort, INPUT);
  pinMode(motionPort, INPUT);
  Serial.begin(115200);

  // setupCamera(); // Uncomment if you plan to use a camera

  initWifi();

  dht.begin();

  client.setServer(mqtt_server, 1883); // กำหนด MQTT Broker และพอร์ต

  // รอให้เซ็นเซอร์เสถียร
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

  // Publish ข้อมูลไปยัง MQTT Broker
  client.publish(mqtt_topic_temp, String(temp).c_str());
  client.publish(mqtt_topic_humid, String(humid).c_str());
}

void readRain() {
  int rain = analogRead(rainPort);
  Serial.print("Current rain : ");
  Serial.println(rain);

  // Publish ข้อมูลไปยัง MQTT Broker
  client.publish(mqtt_topic_rain, String(rain).c_str());
}

void readMotion() {
  int motion = digitalRead(motionPort);

  if (motion == LOW) {
    Serial.println("No Motion");
    client.publish(mqtt_topic_motion, "No Motion");
  } else {
    Serial.println("Motion Detected");
    client.publish(mqtt_topic_motion, "Motion Detected");
  }
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop(); // ฟังก์ชันนี้ใช้ประมวลผล MQTT Callback และเชื่อมต่อใหม่ถ้าจำเป็น

  readTemp();
  readRain();
  readMotion();
  Serial.println();
  delay(2000);  // Delay between readings
}
