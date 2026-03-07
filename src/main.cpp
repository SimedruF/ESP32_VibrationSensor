#include <Arduino.h>
#include "WiFiWebManager.h"
#include "WebPages.h"

// ESP32 pin configuration
static const uint8_t VIBRATION_PIN = 14;  // KY-002 connected to GPIO14
static const uint8_t LED_PIN = 2;

// WiFi Web Manager instance
WiFiWebManager wifiManager("ESP32_VibrationSensor", "12345678", 80);

// Variables for vibration sensor
int vibrationState = 0;           // Current state (0=no vibration, 1=vibration detected)
int lastVibrationState = 0;       // Previous state
unsigned long vibrationCount = 0; // Total vibration events
unsigned long lastVibrationTime = 0; // Last detection time

// Hold time for vibration indicator (to keep LED on briefly)
const unsigned long VIBRATION_HOLD_TIME = 200; // 200ms hold
unsigned long vibrationHoldUntil = 0;

// Circular buffer for chart (last 100 readings for 1 second history)
const int BUFFER_SIZE = 100;
int vibrationBuffer[BUFFER_SIZE];
int bufferIndex = 0;
bool bufferFull = false;

// Handler for main page
void handleRoot(WebServer& server) {
  server.send(200, "text/html", htmlPage);
}

// Handler for JSON data (AJAX endpoint)
void handleData(WebServer& server) {
  String json = "{";
  json += "\"state\":" + String(vibrationState) + ",";
  json += "\"count\":" + String(vibrationCount) + ",";
  
  if (lastVibrationTime > 0) {
    unsigned long timeAgo = (millis() - lastVibrationTime) / 1000;
    if (timeAgo < 60) {
      json += "\"lastTime\":\"" + String(timeAgo) + "s\",";
    } else {
      json += "\"lastTime\":\"" + String(timeAgo / 60) + "m " + String(timeAgo % 60) + "s\",";
    }
  } else {
    json += "\"lastTime\":\"-\",";
  }
  
  // Add buffer data for chart
  json += "\"history\":[";
  
  // Send all buffer data in correct order
  int count = bufferFull ? BUFFER_SIZE : bufferIndex;
  for (int i = 0; i < count; i++) {
    int idx = bufferFull ? ((bufferIndex + i) % BUFFER_SIZE) : i;
    json += String(vibrationBuffer[idx]);
    if (i < count - 1) json += ",";
  }
  
  json += "]}";
  
  server.send(200, "application/json", json);
}

void setup() { 
  Serial.begin(115200); 
  delay(200); 
  
  Serial.println("\n=== ESP32 + KY-002 Vibration Sensor ===");
  Serial.println("Senzor conectat pe GPIO 14");
  
  // LED configuration
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Vibration sensor configuration
  pinMode(VIBRATION_PIN, INPUT_PULLUP); // KY-002 cu pull-up intern
  
  // Initialize buffer with zeros
  for (int i = 0; i < BUFFER_SIZE; i++) {
    vibrationBuffer[i] = 0;
  }
  
  // LED test
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize WiFi and Web Server
  wifiManager.begin();
  
  // Register custom routes
  wifiManager.on("/", HTTP_GET, handleRoot);
  wifiManager.on("/data", HTTP_GET, handleData);
  
  Serial.println("✅ Vibration monitoring system ready!");
  Serial.println("========================================\n");
} 

void loop() 
{ 
  // Process HTTP requests (handled by WiFiWebManager)
  wifiManager.handleClient();
  
  // Read vibration sensor (KY-002 is digital: HIGH = no vibration, LOW = vibration detected)
  int rawReading = digitalRead(VIBRATION_PIN);
  int currentReading = (rawReading == LOW) ? 1 : 0;
  
  // Detect vibration event (ANY detection counts, even if very short)
  if (currentReading == 1) {
    // New vibration detected!
    if (lastVibrationState == 0) {
      vibrationCount++;
      Serial.println("⚠️ VIBRAȚIE DETECTATĂ! #" + String(vibrationCount));
    }
    
    lastVibrationTime = millis();
    vibrationHoldUntil = millis() + VIBRATION_HOLD_TIME;
    vibrationState = 1;
    
    // Turn on LED
    digitalWrite(LED_PIN, HIGH);
  } else {
    // No vibration detected on pin
    // But keep state active if we're in hold period
    if (millis() < vibrationHoldUntil) {
      vibrationState = 1; // Keep showing as active
    } else {
      vibrationState = 0;
      digitalWrite(LED_PIN, LOW);
    }
  }
  
  // Add current state to circular buffer
  vibrationBuffer[bufferIndex] = vibrationState;
  bufferIndex++;
  if (bufferIndex >= BUFFER_SIZE) {
    bufferIndex = 0;
    bufferFull = true;
  }
  
  lastVibrationState = currentReading;
  
  delay(10); // 10ms = 100 readings/second for high sensitivity
}