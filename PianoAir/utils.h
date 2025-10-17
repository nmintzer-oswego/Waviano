#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "config.h"
#include "note_mapping.h"

// ============================================
// ULTRASONIC SENSOR STATE
// ============================================

// Timing variables for ultrasonic sensor
unsigned long last_time_ultrasonic_trigger = 0;

// Pulse timing for distance calculation
volatile unsigned long pulse_in_begin = 0;
volatile unsigned long pulse_in_end = 0;
volatile bool new_distance_available = false;

// ============================================
// ULTRASONIC SENSOR FUNCTIONS
// ============================================

/**
 * Trigger the ultrasonic sensor to send a pulse
 */
void triggerUltrasonicSensor() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
}

/**
 * Interrupt handler for echo pin
 * Measures pulse duration for distance calculation
 */
void echo_pin_interrupt() {
  if (digitalRead(ECHO_PIN) == HIGH) {
    pulse_in_begin = micros();
  } else {
    pulse_in_end = micros();
    new_distance_available = true;
  }
}

/**
 * Calculate distance from ultrasonic pulse timing
 * @return Distance in centimeters
 */
float getDistance() {
  unsigned long duration_micros = pulse_in_end - pulse_in_begin;
  return duration_micros / 58.0;  // Convert to cm
}

/**
 * Check if new distance measurement is available
 * @return true if new measurement ready
 */
bool isNewDistanceAvailable() {
  return new_distance_available;
}

/**
 * Clear the new distance flag
 */
void clearDistanceFlag() {
  new_distance_available = false;
}

/**
 * Update ultrasonic sensor (call in main loop)
 * Triggers sensor at regular intervals
 */
void updateUltrasonicSensor() {
  unsigned long time_now = millis();
  if (time_now - last_time_ultrasonic_trigger > ULTRASONIC_TRIGGER_DELAY) {
    last_time_ultrasonic_trigger = time_now;
    triggerUltrasonicSensor();
  }
}

// ============================================
// LED CONTROL FUNCTIONS
// ============================================

/**
 * Turn off all LEDs
 */
void turnOffAllLEDs() {
  digitalWrite(LED_Do, LOW);
  digitalWrite(LED_Re, LOW);
  digitalWrite(LED_Mi, LOW);
  digitalWrite(LED_Fa, LOW);
  digitalWrite(LED_Sol, LOW);
  digitalWrite(LED_La, LOW);
  digitalWrite(LED_Si, LOW);
  digitalWrite(LED_Do_High, LOW);
}

/**
 * Light up LED for specific note
 * @param note_index Note index (0-7)
 */
void lightUpNoteLED(int note_index) {
  int led_pin = getNoteLED(note_index);
  if (led_pin != -1) {
    digitalWrite(led_pin, HIGH);
  }
}

/**
 * Light up LED for specific note (turn off others first)
 * @param note_index Note index (0-7)
 */
void setNoteLED(int note_index) {
  turnOffAllLEDs();
  lightUpNoteLED(note_index);
}

/**
 * Turn off LED for specific note
 * @param note_index Note index (0-7)
 */
void turnOffNoteLED(int note_index) {
  int led_pin = getNoteLED(note_index);
  if (led_pin != -1) {
    digitalWrite(led_pin, LOW);
  }
}

// ============================================
// BUZZER CONTROL FUNCTIONS
// ============================================

/**
 * Play a note on the buzzer
 * @param note_index Note index (0-7)
 */
void playNote(int note_index) {
  int frequency = getNoteFrequency(note_index);
  if (frequency > 0) {
    tone(BUZZER_PIN, frequency);
  }
}

/**
 * Stop playing note on buzzer
 */
void stopNote() {
  noTone(BUZZER_PIN);
}

/**
 * Play a note for a specific duration
 * @param note_index Note index (0-7)
 * @param duration_ms Duration in milliseconds
 */
void playNoteWithDuration(int note_index, unsigned int duration_ms) {
  playNote(note_index);
  setNoteLED(note_index);
  delay(duration_ms);
  stopNote();
}

// ============================================
// INITIALIZATION FUNCTIONS
// ============================================

/**
 * Initialize all hardware pins
 */
void initializeHardware() {
  // Initialize ultrasonic sensor pins
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  // Initialize LED pins
  pinMode(LED_Do, OUTPUT);
  pinMode(LED_Re, OUTPUT);
  pinMode(LED_Mi, OUTPUT);
  pinMode(LED_Fa, OUTPUT);
  pinMode(LED_Sol, OUTPUT);
  pinMode(LED_La, OUTPUT);
  pinMode(LED_Si, OUTPUT);
  pinMode(LED_Do_High, OUTPUT);

  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);

  // Turn off all LEDs initially
  turnOffAllLEDs();

  // Attach interrupt for echo pin
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echo_pin_interrupt, CHANGE);
}

#endif // UTILS_H
