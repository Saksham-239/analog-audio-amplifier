/**
 * @file main.cpp
 * @brief Professional FreeRTOS-based ESP32 Firmware for Analog Audio Amplifier.
 * 
 * This firmware implements a multitasking architecture on the ESP32:
 *  - Core 1: Runs a high-priority task for decoding MP3 files from SD Card 
 *            and feeding the I2S DAC (glitch-free audio).
 *  - Core 0: Runs a low-priority task for triggering and filtering the dual 
 *            HC-SR04 ultrasonic sensors to adjust volume and control the mute relay.
 * 
 * Standardizing on software volume control (audio.setVolume) removes the requirement
 * for an external digital potentiometer chip, optimizing both the PCB footprint and BOM.
 */

#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>

// --- Pin Configurations ---
#define SD_CS         5
#define I2S_BCLK      26
#define I2S_LRC       25
#define I2S_DOUT      22

// Proximity Sensor 1: Volume Mapping
#define TRIG_VOL_PIN  17
#define ECHO_VOL_PIN  16

// Proximity Sensor 2: Power/Mute Relay Toggling
#define TRIG_MUTE_PIN 18
#define ECHO_MUTE_PIN 19

// Speaker Mute Relay Control
#define RELAY_PIN     2

// --- Global Audio Object ---
Audio audio;

// --- FreeRTOS Task & Sync Handles ---
TaskHandle_t AudioTaskHandle = NULL;
TaskHandle_t SensorTaskHandle = NULL;
SemaphoreHandle_t AudioMutex = NULL;

// --- Helper Functions ---
float readDistance(uint8_t trigPin, uint8_t echoPin);

/**
 * @brief High-priority task running on Core 1 dedicated to streaming and decoding audio.
 */
void AudioPlaybackTask(void *pvParameters) {
  Serial.printf("[Task] AudioPlaybackTask started on Core %d\n", xPortGetCoreID());
  
  for (;;) {
    // Take mutex briefly to service the audio stream loop
    if (xSemaphoreTake(AudioMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
      audio.loop();
      xSemaphoreGive(AudioMutex);
    }
    // Yield briefly to prevent watchdog triggers and allow context switches
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

/**
 * @brief Periodic task running on Core 0 handling ultrasonic sensors and control logic.
 */
void SensorControlTask(void *pvParameters) {
  Serial.printf("[Task] SensorControlTask started on Core %d\n", xPortGetCoreID());

  float smoothVolDist = -1.0f;
  float smoothMuteDist = -1.0f;
  
  for (;;) {
    // --- 1. Touchless Volume Sensor ---
    float rawVolDist = readDistance(TRIG_VOL_PIN, ECHO_VOL_PIN);
    if (rawVolDist > 5.0f && rawVolDist < 80.0f) {
      // Apply Exponential Moving Average (EMA) to filter out noise
      if (smoothVolDist < 0) {
        smoothVolDist = rawVolDist;
      } else {
        smoothVolDist = (0.70f * smoothVolDist) + (0.30f * rawVolDist);
      }

      // Map smoothed distance (5cm - 80cm) to audio volume (21 - 0)
      int targetVol = map((int)smoothVolDist, 5, 80, 21, 0);
      targetVol = constrain(targetVol, 0, 21);

      if (xSemaphoreTake(AudioMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        audio.setVolume(targetVol);
        xSemaphoreGive(AudioMutex);
      }
      Serial.printf("[Sensor-Vol] Distance: %.1f cm -> Target Vol: %d\n", smoothVolDist, targetVol);
    }

    // --- 2. Touchless Mute Relay Sensor ---
    float rawMuteDist = readDistance(TRIG_MUTE_PIN, ECHO_MUTE_PIN);
    if (rawMuteDist > 2.0f && rawMuteDist < 100.0f) {
      // Apply Exponential Moving Average (EMA) for mute tracking
      if (smoothMuteDist < 0) {
        smoothMuteDist = rawMuteDist;
      } else {
        smoothMuteDist = (0.80f * smoothMuteDist) + (0.20f * rawMuteDist);
      }

      // If hand is detected close (<30cm), unmute; otherwise mute
      if (smoothMuteDist < 30.0f) {
        digitalWrite(RELAY_PIN, HIGH); // Unmute/Power ON
        Serial.printf("[Sensor-Mute] Distance: %.1f cm -> UNMUTED (Relay ON)\n", smoothMuteDist);
      } else {
        digitalWrite(RELAY_PIN, LOW);  // Mute/Power OFF (default)
        Serial.printf("[Sensor-Mute] Distance: %.1f cm -> MUTED (Relay OFF)\n", smoothMuteDist);
      }
    }

    // Sample sensors at 10Hz (every 100ms)
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/**
 * @brief Triggers an HC-SR04 sensor and calculates distance based on echo response time.
 */
float readDistance(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read high pulse with 30ms timeout (~5m max range)
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) {
    return -1.0f; // Timeout or sensor read error
  }
  
  // Calculate distance in cm (Speed of sound = 343 m/s -> 0.0343 cm/us)
  return (float)duration * 0.0343f / 2.0f;
}

void setup() {
  Serial.begin(115200);

  // Initialize SPI for SD Card (Standard VSPI Bus pins)
  SPI.begin(18, 19, 23);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  if (!SD.begin(SD_CS)) {
    Serial.println("[-] Initializing SD Card failed! Retrying...");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("[+] SD Card initialized successfully.");

  // Configure hardware pins
  pinMode(TRIG_VOL_PIN, OUTPUT);
  pinMode(ECHO_VOL_PIN, INPUT);
  pinMode(TRIG_MUTE_PIN, OUTPUT);
  pinMode(ECHO_MUTE_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Set initial safe state (muted)
  digitalWrite(RELAY_PIN, LOW);

  // Create Mutex for shared access to the Audio object
  AudioMutex = xSemaphoreCreateMutex();
  if (AudioMutex == NULL) {
    Serial.println("[-] Error creating Audio Mutex!");
    while (1) delay(1000);
  }

  // Initialize I2S DAC Pinout
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(5);
  audio.connecttoFS(SD, "/music/track.mp3");

  // Create thread-safe tasks on specific cores
  xTaskCreatePinnedToCore(
    AudioPlaybackTask,      // Task function
    "AudioPlaybackTask",    // Task name
    8192,                   // Stack size (bytes)
    NULL,                   // Task input parameter
    5,                      // Priority (Highest)
    &AudioTaskHandle,       // Task handle
    1                       // Core pinned to (Core 1)
  );

  xTaskCreatePinnedToCore(
    SensorControlTask,
    "SensorControlTask",
    4096,
    NULL,
    1,                      // Priority (Lowest)
    &SensorTaskHandle,
    0                       // Core pinned to (Core 0)
  );
}

void loop() {
  // Since tasks are running concurrently in FreeRTOS, delete the default loop task
  vTaskDelete(NULL);
}
