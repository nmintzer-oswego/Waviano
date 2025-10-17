/**
 * PianoAir - Air Piano with Ultrasonic Sensor
 *
 * Features:
 * - Free play mode: Play any notes freely
 * - Recording: Record your performances to 4 slots
 * - Multi-track playback: Play back recordings individually or merged
 *
 * Hardware:
 * - Ultrasonic sensor (HC-SR04)
 * - 8 LEDs for note indication
 * - Buzzer for sound output
 */

#include "config.h"
#include "note_mapping.h"
#include "utils.h"
#include "recording.h"
#include "playback.h"
#include "ui.h"

// ============================================
// GLOBAL STATE
// ============================================

// Current system mode
SystemMode current_mode = MODE_MENU;

// Currently selected recording slot for recording
int current_recording_slot = -1;

// Overlap strategy for multi-track playback
OverlapStrategy overlap_strategy = DEFAULT_OVERLAP_STRATEGY;

// Last detected note for debouncing
int last_detected_note = -1;
unsigned long last_detected_note_time = 0;

// ============================================
// SETUP
// ============================================

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize hardware (pins, interrupts)
  initializeHardware();

  // Initialize recording system
  initializeRecordingSystem();

  // Print welcome message and menu
  Serial.println(F("\n\n"));
  Serial.println(F("*****************************************"));
  Serial.println(F("*                                       *"));
  Serial.println(F("*         WELCOME TO PIANO AIR!         *"));
  Serial.println(F("*                                       *"));
  Serial.println(F("*****************************************"));

  printMainMenu();
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {
  unsigned long current_time = millis();

  // ---- HANDLE SERIAL INPUT ----
  SystemMode new_mode = processSerialInput();
  if (new_mode != current_mode) {
    current_mode = new_mode;
  }

  // ---- UPDATE ULTRASONIC SENSOR ----
  updateUltrasonicSensor();

  // ---- UPDATE PLAYBACK ----
  if (current_mode == MODE_PLAYBACK) {
    if (!updatePlayback()) {
      // Playback finished
      current_mode = MODE_FREE_PLAY;
      Serial.println(F("\nPlayback finished.\n"));
    }
    return;  // Skip sensor processing during playback
  }

  // ---- PROCESS SENSOR INPUT ----
  if (isNewDistanceAvailable()) {
    clearDistanceFlag();

    float distance = getDistance();
    int note_index = getNoteFromDistance(distance);

    // Valid note detected
    if (note_index != -1) {
      // Debounce: ignore if same note detected too quickly
      bool is_new_note = (note_index != last_detected_note) ||
                         (current_time - last_detected_note_time > NOTE_DEBOUNCE_MS);

      if (is_new_note) {
        last_detected_note = note_index;
        last_detected_note_time = current_time;

        // Handle note based on current mode
        switch (current_mode) {
          case MODE_FREE_PLAY:
            handleFreePlayNote(note_index);
            break;

          case MODE_RECORDING:
            handleRecordingNote(note_index);
            break;

          default:
            break;
        }
      }
    }
  }

}

// ============================================
// NOTE HANDLING FUNCTIONS
// ============================================

/**
 * Handle note in free play mode
 */
void handleFreePlayNote(int note_index) {
  playNoteWithDuration(note_index, NOTE_DURATION_MS);

  #if ENABLE_DEBUG
  Serial.print(F("Note: "));
  Serial.println(getNoteName(note_index, true));
  #endif
}


/**
 * Handle note in recording mode
 */
void handleRecordingNote(int note_index) {
  // Add note to recording
  if (addNoteToRecording(note_index)) {
    playNoteWithDuration(note_index, NOTE_DURATION_MS);

    #if ENABLE_DEBUG
    int slot = getActiveRecordingSlot();
    Serial.print(F("Recorded: "));
    Serial.print(getNoteName(note_index, true));
    Serial.print(F(" ["));
    Serial.print(getSlotNoteCount(slot));
    Serial.print(F("/"));
    Serial.print(MAX_NOTES_PER_SLOT);
    Serial.println(F("]"));
    #endif
  } else {
    // Recording failed (buffer full)
    Serial.println(F("\n*** Recording buffer full! Recording stopped. ***\n"));
    stopRecording();
    current_mode = MODE_FREE_PLAY;
  }
}
