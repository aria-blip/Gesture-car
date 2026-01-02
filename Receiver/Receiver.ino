/*
 * Project: Gesture Control Receiver (Car)
 * Hardware: ESP32 + L298N Motor Driver
 * Feature: Failsafe Auto-Stop
 */

#include <esp_now.h>
#include <WiFi.h>

// --- PIN DEFINITIONS ---
#define MOTOR_A_IN1 32
#define MOTOR_A_IN2 33
#define MOTOR_B_IN1 25
#define MOTOR_B_IN2 26

// --- SAFETY CONFIG ---
#define SIGNAL_TIMEOUT 500 // Stop car if no signal for 500ms

// Data Packet Structure (Must match Transmitter)
typedef struct {
  uint8_t moveFwd;
  uint8_t moveBwd;
  uint8_t moveLeft;
  uint8_t moveRight;
} ControlPacket;

ControlPacket incomingData;
unsigned long lastRecvTime = 0;

// Motor Control Primitives
void stopMotors() {
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, LOW);
  digitalWrite(MOTOR_B_IN1, LOW);
  digitalWrite(MOTOR_B_IN2, LOW);
}

void moveForward() {
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, HIGH);
  digitalWrite(MOTOR_B_IN1, HIGH); // Check wiring polarity
  digitalWrite(MOTOR_B_IN2, LOW);
}

void moveBackward() {
  digitalWrite(MOTOR_A_IN1, HIGH);
  digitalWrite(MOTOR_A_IN2, LOW);
  digitalWrite(MOTOR_B_IN1, LOW);
  digitalWrite(MOTOR_B_IN2, HIGH);
}

void turnLeft() {
  digitalWrite(MOTOR_A_IN1, HIGH); // Tank turn
  digitalWrite(MOTOR_A_IN2, LOW);
  digitalWrite(MOTOR_B_IN1, HIGH);
  digitalWrite(MOTOR_B_IN2, LOW);
}

void turnRight() {
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, HIGH);
  digitalWrite(MOTOR_B_IN1, LOW);
  digitalWrite(MOTOR_B_IN2, HIGH);
}

// Callback when data is received
void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t *incoming, int len) {
  if (len == sizeof(incomingData)) {
    memcpy(&incomingData, incoming, sizeof(incomingData));
    lastRecvTime = millis(); // Reset safety timer
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_B_IN1, OUTPUT);
  pinMode(MOTOR_B_IN2, OUTPUT);
  stopMotors();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart();
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Failsafe Check: Signal Lost?
  if ((millis() - lastRecvTime) > SIGNAL_TIMEOUT) {
    stopMotors();
    return; 
  }

  // Active Control Logic
  if (incomingData.moveFwd)       moveForward();
  else if (incomingData.moveBwd)  moveBackward();
  else if (incomingData.moveLeft) turnLeft();
  else if (incomingData.moveRight) turnRight();
  else stopMotors();
  
  delay(10); // Stability delay
}