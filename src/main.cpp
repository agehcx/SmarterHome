#include <Arduino.h>
#include <WiFi.h>

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
    delay(5000);
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
  // pinMode(rainPort, INPUT);
  // pinMode(tempPort, INPUT);
  // pinMode(motorPort, OUTPUT);
  // pinMode(motionPort, INPUT);
  Serial.begin(115200);

  initWifi();


}

void loop() {
  // put your main code here, to run repeatedly:
}
