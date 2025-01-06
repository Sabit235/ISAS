#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "DHT.h"

// Define pins and constants
#define gasSensorInputPin 33
#define DHTPIN 32
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Structure to send data
typedef struct {
  int gasLevel;
  float temperature;
  float humidity;
} SensorData;

SensorData dataToSend;

// Master ESP32 MAC address
uint8_t masterAddress[] = {0x2C, 0xBC, 0xBB, 0x0D, 0xE3, 0x9C};

void setup() {
  Serial.begin(115200);

  // Initialize gas sensor pin
  pinMode(gasSensorInputPin, INPUT);

  // Initialize DHT sensor
  dht.begin();

  // Initialize Wi-Fi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  Serial.println("ESP-NOW Initialized");

  // Register peer (master ESP32 address)
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Master peer added successfully");
}

void loop() {
  // Read gas sensor value
  dataToSend.gasLevel = analogRead(gasSensorInputPin);

  // Read temperature and humidity
  dataToSend.humidity = dht.readHumidity();
  dataToSend.temperature = dht.readTemperature();

  // Send data via ESP-NOW
  esp_err_t result = esp_now_send(masterAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));
  if (result == ESP_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Error sending data");
  }

  // Debug print
  Serial.print("Gas Level: ");
  Serial.print(dataToSend.gasLevel);
  Serial.print("\tTemperature: ");
  Serial.print(dataToSend.temperature);
  Serial.print(" *C\tHumidity: ");
  Serial.print(dataToSend.humidity);
  Serial.println(" %");

  delay(1000); 
}
