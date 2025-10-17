#ifndef CONFIG_H
#define CONFIG_H

// ============================================
// HARDWARE PIN CONFIGURATION
// ============================================

// Ultrasonic Sensor Pins
#define ECHO_PIN 3
#define TRIGGER_PIN 4

// LED Pins (8 notes)
#define LED_Do 13
#define LED_Re 12
#define LED_Mi 11
#define LED_Fa 10
#define LED_Sol 9
#define LED_La 8
#define LED_Si 7
#define LED_Do_High 6

// Buzzer Pin
#define BUZZER_PIN 2

// ============================================
// TIMING CONSTANTS
// ============================================

// Ultrasonic sensor trigger interval (ms)
#define ULTRASONIC_TRIGGER_DELAY 100

// Note duration for free play mode (ms)
#define NOTE_DURATION_MS 500

// Debounce time for note detection (ms)
#define NOTE_DEBOUNCE_MS 50

// ============================================
// RECORDING CONFIGURATION
// ============================================

// Number of recording slots available
#define NUM_RECORDING_SLOTS 4

// Maximum notes per recording slot
// Note: Each note takes ~2 bytes (note_index + duration)
// 30 notes × 2 bytes × 4 slots = 240 bytes
#define MAX_NOTES_PER_SLOT 30

// Duration unit for recording (ms)
// Durations stored as multiples of this value
#define DURATION_UNIT_MS 100

// Maximum duration per note in units (255 × 100ms = 25.5 seconds)
#define MAX_NOTE_DURATION_UNITS 255

// ============================================
// PLAYBACK CONFIGURATION
// ============================================

// Overlap resolution strategies
enum OverlapStrategy {
  OVERLAP_PRIORITY_HIGH = 0,  // Play highest note when overlap
  OVERLAP_PRIORITY_LOW = 1,   // Play lowest note when overlap
  OVERLAP_ALTERNATE = 2,      // Rapidly alternate between notes
  OVERLAP_DROP = 3            // First note takes priority, drop others
};

// Default overlap strategy
#define DEFAULT_OVERLAP_STRATEGY OVERLAP_PRIORITY_HIGH

// Alternate mode switching interval (ms)
#define ALTERNATE_SWITCH_INTERVAL_MS 50

// ============================================
// SYSTEM MODES
// ============================================

enum SystemMode {
  MODE_MENU = 0,        // Main menu / idle
  MODE_GUIDED = 1,      // Following pre-programmed song
  MODE_FREE_PLAY = 2,   // Playing notes freely
  MODE_RECORDING = 3,   // Recording in progress
  MODE_PLAYBACK = 4     // Playing back recording(s)
};

// ============================================
// FEATURE FLAGS
// ============================================

// Enable EEPROM storage (future feature)
#define ENABLE_EEPROM false

// Enable debug output
#define ENABLE_DEBUG false

// ============================================
// GLOBAL STATE VARIABLES
// ============================================

// Current system mode
extern SystemMode current_mode;

// Currently selected recording slot for recording
extern int current_recording_slot;

// Overlap strategy for multi-track playback
extern OverlapStrategy overlap_strategy;

#endif // CONFIG_H
