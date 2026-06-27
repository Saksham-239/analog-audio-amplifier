/**
 * @file firmware_basic.ino
 * @brief Basic Arduino sketch for ESP32 MP3 player with Ultrasonic Volume & Mute control.
 * 
 * This code implements software-based volume control and muting using the ESP32-audioI2S 
 * library. An SD card holds the MP3 file, which is decoded in software and output over 
 * I2S pins. Dual HC-SR04 ultrasonic sensors provide touchless control:
 *  - Sensor 1 (Volume): Closer hand -> higher volume.
 *  - Sensor 2 (Mute): Hand within 30cm -> unmute speaker (Relay ON).
 */

#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <FS.h>
#include <Wire.h>

// Pin Definitions
#define SD_CS         5
#define I2S_BCLK      26
#define I2S_LRC       25
#define I2S_DOUT      22

// Sensor 1: Volume Control
#define TRIG_PIN      17
#define ECHO_PIN      16

// Sensor 2: Mute Control
#define TRIG_PIN2     18
#define ECHO_PIN2     19

// Mute Relay Pin
#define RELAY_PIN     2

Audio audio;

void setup() {
  Serial.begin(115200);

  // Initialize SPI bus for SD Card (Standard VSPI pins: SCK=18, MISO=19, MOSI=23)
  SPI.begin(18, 19, 23);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  if (!SD.begin(SD_CS)) {
    Serial.println("[-] SD Card mount failed! Check connections.");
    while (1) {
      delay(500);
    }
  }
  Serial.println("[+] SD Card mounted successfully.");

  // Configure I2S DAC Pinout
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(5); // Default startup volume (0 to 21 scale)
  
  // Play track.mp3 from the music directory
  audio.connecttoFS(SD, "/music/track.mp3");

  // Configure sensor and relay pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Muted by default at startup
}

void loop() {
  // Service audio buffer (needs to be called as frequently as possible)
  audio.loop();

  // Run sensor checks periodically (every 100ms) to avoid starving the audio buffer
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 100) {
    lastSensorRead = millis();

    // --- 1. Read Volume Sensor (HC-SR04 #1) ---
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long dur = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout (max ~5 meters)
    float dist = dur * 0.034 / 2.0;

    // Check if the hand is within sensing bounds (5 cm to 80 cm)
    if (dist > 5 && dist < 80) {
      // Map distance: 5cm (closest) -> Vol 21 (max); 80cm (farthest) -> Vol 0 (min)
      int vol = map((int)dist, 5, 80, 21, 0);
      audio.setVolume(constrain(vol, 0, 21));
      Serial.printf("[Volume] Distance: %.1f cm | Set Vol: %d\n", dist, vol);
    }

    // --- 2. Read Mute/Presence Sensor (HC-SR04 #2) ---
    digitalWrite(TRIG_PIN2, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN2, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN2, LOW);
    
    long dur2 = pulseIn(ECHO_PIN2, HIGH, 30000);
    float dist2 = dur2 * 0.034 / 2.0;

    // Unmute speaker if a hand/object is detected close by (<30 cm)
    if (dist2 > 2 && dist2 < 30) {
      digitalWrite(RELAY_PIN, HIGH); // Unmute/Power ON
      Serial.printf("[Mute] Distance: %.1f cm | Status: UNMUTED (Relay ON)\n", dist2);
    } else {
      digitalWrite(RELAY_PIN, LOW);  // Mute/Power OFF
      Serial.printf("[Mute] Distance: %.1f cm | Status: MUTED (Relay OFF)\n", dist2);
    }
  }
}
