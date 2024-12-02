#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6xCkV8Snl"
#define BLYNK_TEMPLATE_NAME "EMBEDDED"
#define BLYNK_AUTH_TOKEN "iARcsUUbGFqPttq42S21ntVZGuqmJ3Us"

#include <WiFi.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include "camera.h"
#include <BlynkSimpleEsp32.h>
#include <LittleFS.h>
#include <SD.h>
#include <Update.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

Servo myServo;

#define DHTTYPE DHT11
const int dhtPin = 33;
DHT dht(dhtPin, DHTTYPE);

int motionPort = 14; // Motion Sensor on GPIO14
int rainPort = 34;   // Rain Sensor on GPIO34

double humid = 0.0;
double temp = 0.0;
int rain = 0;
double rainRes = 0;
int motion = 0;

String notiString;

bool tempHighNotified = false;
bool humidHighNotified = false;
bool heavyRainNotified = false;
bool coldHumidNotified = false;
bool sunnyWeatherNotified = false;

// WiFi credentials
const char* ssid1 = "Chega";
const char* password1 = "qazplmvgty";

String Web_App_URL = "https://script.google.com/macros/s/AKfycbwwdXNFh0b6xPx5jpKHW2HFaLVG0IUVpLpHn184QvPR1YIw6azxmNLRuzu-WeN7M4RH/exec";

char auth[] = "iARcsUUbGFqPttq42S21ntVZGuqmJ3Us";

// Notification token for LINE API
String token = "jYxTfLQHXiwfHtUPYht7s1fYVVHCJm42qE91qXUSxAC";

bool hasClosedWindow = false;

// WiFi initialization function with reconnection logic
void initWifi() {
    Serial.print("\nConnecting to WiFi");
    WiFi.begin(ssid1, password1);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to the WiFi network: " + String(ssid1));
        Serial.print("Station IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi. Check credentials and network.");
        ESP.restart();
    }
}

void readMotion() {
  motion = digitalRead(motionPort);

  if (motion == LOW) {
    Serial.println("No motion detected");
    Blynk.virtualWrite(V3, "No Motion"); // Send motion status to Virtual Pin V3
  } else {
    Serial.println("Motion detected");
    Blynk.virtualWrite(V3, "Motion Detected");
  }

  
}



// Function to send a notification via LINE
void sendLineNotification(String message) {
    HTTPClient http;
    http.begin("https://notify-api.line.me/api/notify");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Bearer " + token);

    String postData = "message=" + message;
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
        Serial.println("Notification sent successfully");
    } else {
        Serial.println("Error sending notification: " + String(httpResponseCode));
    }

    http.end();
}

// Function to send data to Google Sheets
void sendToGoogleSheet(String url, String data) {
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST("data=" + data);
    Serial.println("HTTP Response code: " + String(httpResponseCode));

    if (httpResponseCode == 302) {
        String locationHeader = http.header("Location");
        Serial.println("Redirect location: " + locationHeader);
    }

    http.end();
}

// Function to read temperature and humidity from the DHT sensor
void readTemp() {
    temp = dht.readTemperature();
    humid = dht.readHumidity();

    if (isnan(temp) || isnan(humid)) {
        Serial.println("\nFailed to read from DHT sensor!");
        return;
    }

    Serial.print("Current temp: ");
    Serial.print(temp);
    Serial.print(" °C");
    Serial.print("\nCurrent humidity: ");
    Serial.print(humid);
    Serial.println(" %");

    // Send data to Blynk
    Blynk.virtualWrite(V0, temp);  // Send temperature to Virtual Pin V0
    Blynk.virtualWrite(V1, humid); // Send humidity to Virtual Pin V1
}

// Function to read rain sensor data
void readRain() {
    rain = analogRead(rainPort);
    Serial.print("Current rain sensor value: ");
    Serial.println(rain);

    double result = (4095 - rain) / 4095.0 * 100; // Convert to percentage

    if (rain < 1500 && !hasClosedWindow) {
        Serial.println("Rain detected, closing window.");
        myServo.write(110); // Close the window
        delay(1000);
        hasClosedWindow = true;
        notiString = "Raining: " + String(result, 2);
        sendLineNotification(notiString);
    } else if (rain >= 4000 && hasClosedWindow) {
        Serial.println("No rain detected, opening window.");
        myServo.write(10); // Open the window
        hasClosedWindow = false;
    }

    // Send rain sensor data to Blynk
    rainRes = result;
    Blynk.virtualWrite(V2, result); // Send rain sensor data to Virtual Pin V2
}


void evaluateRules() {
    if (temp > 30.0) {
        if (!tempHighNotified) {
            Serial.println("Temperature is too high! Taking action.");
            sendLineNotification("Warning: Temperature exceeds 30°C.");
            tempHighNotified = true;
        }
    } else {
        tempHighNotified = false;
    }

    if (humid > 70.0) {
        if (!humidHighNotified) {
            Serial.println("High humidity detected! Notifying user.");
            sendLineNotification("Humidity Alert: Exceeds 70%.");
            humidHighNotified = true;
        }
    } else {
        humidHighNotified = false;
    }

    if (rainRes > 80.0) {
        if (!heavyRainNotified) {
            Serial.println("Heavy rain detected! Sending alert.");
            sendLineNotification("Heavy rain warning: Rain intensity exceeds 80%.");
            heavyRainNotified = true;
        }
    } else {
        heavyRainNotified = false;
    }

    if (temp < 10.0 && humid > 50.0) {
        if (!coldHumidNotified) {
            Serial.println("Cold and humid conditions detected. Sending notification.");
            sendLineNotification("Cold and humid conditions: Temperature below 10°C, Humidity above 50%.");
            coldHumidNotified = true;
        }
    } else {
        coldHumidNotified = false;
    }

    if (rainRes < 10.0 && temp > 25.0) {
        if (!sunnyWeatherNotified) {
            Serial.println("Sunny conditions detected. Suggesting open windows.");
            sendLineNotification("Sunny weather detected. Consider opening windows.");
            sunnyWeatherNotified = true;
        }
    } else {
        sunnyWeatherNotified = false;
    }
}

// Function to initialize and test the servo
void setServo() {
    delay(1000);
    myServo.write(0);
    delay(1000);
    myServo.write(90);
}

void setup() {
    pinMode(rainPort, INPUT);
    pinMode(motionPort, INPUT);
    Serial.begin(115200);

    // Disable brown-out detection for stability
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // Initialize the servo
    myServo.attach(13); // Attach the servo to GPIO13

    // Initialize Wi-Fi
    initWifi();

    // Initialize DHT sensor
    dht.begin();

    // Start Blynk
    Blynk.begin(auth, ssid1, password1);

    // Wait for sensor stabilization
    delay(2000);
}

void loop() {
    Blynk.run(); // Run Blynk

    // Read sensor data
    readTemp();
    readRain();
    readMotion();

    // Evaluate rules based on the sensor data
    evaluateRules();
    Serial.println();
    delay(2000); // Delay between readings (2 seconds)

    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL = Send_Data_URL + "&humid=" + String(humid);
    Send_Data_URL = Send_Data_URL + "&temp=" + String(temp);
    Send_Data_URL = Send_Data_URL + "&rain=" + String(rainRes);
    Send_Data_URL = Send_Data_URL + "&motion=" + String(motion);
    // Initialize HTTPClient as "http".
    HTTPClient http;

    // HTTP GET Request.
    http.begin(Send_Data_URL.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    // Gets the HTTP status code.
    int httpCode = http.GET();
    http.end();
}
