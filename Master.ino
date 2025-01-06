#include <esp_now.h>
#include <WiFi.h>

// Define LED pins for each slave
int ledPinSlave1 = 23; // LED for Slave 1
int ledPinSlave2 = 22; // LED for Slave 2
int gasThreshold = 2000;

// Structure to receive data
typedef struct {
  int gasLevel;
  float temperature;
  float humidity;
} SensorData;

SensorData receivedData;

// Slave MAC addresses
uint8_t slave1Address[] = {0x2C, 0xBC, 0xBB, 0x0D, 0x91, 0x18};
uint8_t slave2Address[] = {0x2C, 0xBC, 0xBB, 0x0D, 0xC1, 0x94};

// Callback function to handle received data
void onDataReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  // Identify the sender using `src_addr`
  if (memcmp(recvInfo->src_addr, slave1Address, 6) == 0) {
    Serial.println("Data from Slave 1:");
    if (receivedData.gasLevel > gasThreshold) {
      digitalWrite(ledPinSlave1, HIGH);
    } else {
      digitalWrite(ledPinSlave1, LOW);
    }
  } else if (memcmp(recvInfo->src_addr, slave2Address, 6) == 0) {
    Serial.println("Data from Slave 2:");
    if (receivedData.gasLevel > gasThreshold) {
      digitalWrite(ledPinSlave2, HIGH);
    } else {
      digitalWrite(ledPinSlave2, LOW);
    }
  }

  // Debug print
  Serial.print("Gas Level: ");
  Serial.print(receivedData.gasLevel);
  Serial.print("\tTemperature: ");
  Serial.print(receivedData.temperature);
  Serial.print(" *C\tHumidity: ");
  Serial.print(receivedData.humidity);
  Serial.println(" %");
}

void setup() {
  Serial.begin(115200);

  // Initialize LED pins
  pinMode(ledPinSlave1, OUTPUT);
  pinMode(ledPinSlave2, OUTPUT);

  // Initialize Wi-Fi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  Serial.println("ESP-NOW Initialized");

  // Register callback for receiving data
  esp_now_register_recv_cb(onDataReceive);

  // Register peers
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  memcpy(peerInfo.peer_addr, slave1Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add Slave 1");
    return;
  }
  Serial.println("Slave 1 added successfully");

  memcpy(peerInfo.peer_addr, slave2Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add Slave 2");
    return;
  }
  Serial.println("Slave 2 added successfully");

  Serial.println("Waiting for data...");
}

void loop() {
  // Do nothing, data is processed in the callback
}
