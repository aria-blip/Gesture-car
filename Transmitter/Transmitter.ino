/*
 * Project: Gesture Control Transmitter
 * Hardware: ESP32 + MPU6050
 * Protocol: ESP-NOW
 */

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <MPU6050_light.h>

// --- CONFIGURATION ---
// REPLACE WITH YOUR RECEIVER MAC ADDRESS
uint8_t receiverMac[] = {0x3C, 0x61, 0x05, 0x1B, 0xCF, 0x58}; 
#define TILT_THRESHOLD 30.0  // Angle in degrees to trigger movement

// Data Packet Structure (Packed for efficiency)
typedef struct {
  uint8_t moveFwd;
  uint8_t moveBwd;
  uint8_t moveLeft;
  uint8_t moveRight;
} ControlPacket;

ControlPacket payload;
esp_now_peer_info_t peerInfo;
MPU6050 mpu(Wire);

unsigned long lastTime = 0;

// Callback: Confirmation of sent data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Optional: Add LED blink here for status
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    ESP.restart();
  }

  esp_now_register_send_cb(OnDataSent);

  // Register Peer
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Init MPU6050
  byte status = mpu.begin();
  if(status != 0) {
    Serial.println("MPU6050 connect failed");
    while(1) delay(10); // Halt
  }
  
  Serial.println("Calibrating... Do not move.");
  delay(1000);
  mpu.calcOffsets();
  Serial.println("Ready.");
}

void loop() {
  mpu.update();

  // Non-blocking timer: 20ms update rate (50Hz)
  if (millis() - lastTime > 20) {
    
    float angleX = mpu.getAngleX();
    float angleY = mpu.getAngleY();

    // Reset payload
    payload.moveFwd = 0;
    payload.moveBwd = 0;
    payload.moveLeft = 0;
    payload.moveRight = 0;

    // Logic Mapping
    if (angleX >= TILT_THRESHOLD) payload.moveFwd = 1;
    else if (angleX <= -TILT_THRESHOLD) payload.moveBwd = 1;
    
    if (angleY >= TILT_THRESHOLD) payload.moveRight = 1;
    else if (angleY <= -TILT_THRESHOLD) payload.moveLeft = 1;

    // Send Data
    esp_now_send(receiverMac, (uint8_t *) &payload, sizeof(payload));
    lastTime = millis();
  }
}