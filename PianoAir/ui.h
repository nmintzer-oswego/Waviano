#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include "config.h"
#include "note_mapping.h"
#include "songs.h"
#include "recording.h"
#include "playback.h"

// ============================================
// UI STATE
// ============================================

// Current overlap strategy for playback
OverlapStrategy current_overlap_strategy = DEFAULT_OVERLAP_STRATEGY;

// ============================================
// MENU DISPLAY FUNCTIONS
// ============================================

/**
 * Print the main menu
 */
void printMainMenu() {
  Serial.println(F("\n========================================"));
  Serial.println(F("        PIANO AIR - Main Menu"));
  Serial.println(F("========================================"));
  Serial.println(F("\nGUIDED MODE (Follow Along):"));
  Serial.println(F("  1 - Mary Had a Little Lamb"));
  Serial.println(F("  2 - Twinkle Twinkle Little Star"));
  Serial.println(F("  3 - The Wheels on the Bus"));
  Serial.println(F("  4 - Yankee Doodle"));
  Serial.println(F("\nFREE PLAY & RECORDING:"));
  Serial.println(F("  0 - Free play mode (Air Piano)"));
  Serial.println(F("  R[1-4] - Record to slot (e.g., R1, R2)"));
  Serial.println(F("  S - Stop recording"));
  Serial.println(F("\nPLAYBACK:"));
  Serial.println(F("  P[1-4] - Play slot (e.g., P1, P2)"));
  Serial.println(F("  PA - Play all slots (merged)"));
  Serial.println(F("  X - Stop playback"));
  Serial.println(F("\nMANAGEMENT:"));
  Serial.println(F("  L - List all recordings"));
  Serial.println(F("  C[1-4] - Clear slot (e.g., C1, C2)"));
  Serial.println(F("  CA - Clear all recordings"));
  Serial.println(F("  M[1-4] - Set overlap mode (see below)"));
  Serial.println(F("\nOVERLAP MODES:"));
  Serial.println(F("  M1 - Priority High (play highest note)"));
  Serial.println(F("  M2 - Priority Low (play lowest note)"));
  Serial.println(F("  M3 - Alternate (rapid switching)"));
  Serial.println(F("  M4 - Drop (first note wins)"));
  Serial.println(F("========================================\n"));
}

/**
 * Print current system status
 */
void printStatus() {
  Serial.print(F("Mode: "));

  if (isRecording()) {
    Serial.print(F("RECORDING to Slot "));
    Serial.print(getActiveRecordingSlot() + 1);
    int note_count = getSlotNoteCount(getActiveRecordingSlot());
    Serial.print(F(" ["));
    Serial.print(note_count);
    Serial.print(F("/"));
    Serial.print(MAX_NOTES_PER_SLOT);
    Serial.println(F(" notes]"));
  } else if (isPlaying()) {
    Serial.print(F("PLAYING"));
    int current, total;
    if (getPlaybackProgress(&current, &total)) {
      Serial.print(F(" ["));
      Serial.print(current);
      Serial.print(F("/"));
      Serial.print(total);
      Serial.print(F(" events]"));
    }
    Serial.println();
  } else if (getCurrentSong() != NULL) {
    Serial.print(F("GUIDED - "));
    Serial.print(getCurrentSong()->name);
    int current, total;
    if (getSongProgress(&current, &total)) {
      Serial.print(F(" ["));
      Serial.print(current);
      Serial.print(F("/"));
      Serial.print(total);
      Serial.print(F("]"));
    }
    Serial.println();
  } else {
    Serial.println(F("FREE PLAY"));
  }
}

/**
 * List all recordings
 */
void listRecordings() {
  Serial.println(F("\n--- Recording Slots ---"));

  bool has_recordings = false;

  for (int i = 0; i < NUM_RECORDING_SLOTS; i++) {
    Serial.print(F("Slot "));
    Serial.print(i + 1);
    Serial.print(F(": "));

    if (isSlotActive(i)) {
      has_recordings = true;
      int note_count = getSlotNoteCount(i);
      unsigned long duration_ms = getRecordingDuration(i);

      Serial.print(note_count);
      Serial.print(F(" notes, "));
      Serial.print(duration_ms / 1000.0, 1);
      Serial.println(F("s"));
    } else {
      Serial.println(F("[Empty]"));
    }
  }

  if (!has_recordings) {
    Serial.println(F("\nNo recordings found."));
  }

  Serial.println(F("----------------------\n"));
}

/**
 * Print overlap strategy name
 */
void printOverlapStrategy(OverlapStrategy strategy) {
  switch (strategy) {
    case OVERLAP_PRIORITY_HIGH:
      Serial.print(F("Priority High"));
      break;
    case OVERLAP_PRIORITY_LOW:
      Serial.print(F("Priority Low"));
      break;
    case OVERLAP_ALTERNATE:
      Serial.print(F("Alternate"));
      break;
    case OVERLAP_DROP:
      Serial.print(F("Drop"));
      break;
    default:
      Serial.print(F("Unknown"));
  }
}

// ============================================
// COMMAND PARSING FUNCTIONS
// ============================================

/**
 * Parse and handle serial input commands
 * @param input Input character or command
 * @return System mode after command execution
 */
SystemMode handleSerialCommand(char input) {
  // Convert to uppercase for easier parsing
  if (input >= 'a' && input <= 'z') {
    input = input - 32;
  }

  // ---- GUIDED MODE SONG SELECTION ----
  if (input >= '1' && input <= '4') {
    int song_index = input - '1';
    if (selectSong(song_index)) {
      Serial.print(F("\nNow playing: "));
      Serial.println(getCurrentSong()->name);
      Serial.println(F("Follow the LED and play the notes!\n"));
      return MODE_GUIDED;
    }
  }

  // ---- FREE PLAY MODE ----
  else if (input == '0') {
    clearSongSelection();
    Serial.println(F("\nFree play mode activated!"));
    return MODE_FREE_PLAY;
  }

  // ---- RECORDING COMMANDS ----
  else if (input == 'R') {
    // Check for slot number in next character
    if (Serial.available() > 0) {
      char slot_char = Serial.read();
      if (slot_char >= '1' && slot_char <= '0' + NUM_RECORDING_SLOTS) {
        int slot_num = slot_char - '1';
        if (startRecording(slot_num)) {
          Serial.print(F("\nRecording to Slot "));
          Serial.print(slot_num + 1);
          Serial.println(F("... Play some notes!"));
          Serial.println(F("Press 'S' to stop recording.\n"));
          return MODE_RECORDING;
        }
      }
    }
    Serial.println(F("\nUsage: R[1-4] (e.g., R1, R2, R3, R4)"));
  }

  else if (input == 'S') {
    if (stopRecording()) {
      Serial.println(F("\nRecording stopped."));
      int slot = getActiveRecordingSlot();
      if (slot >= 0) {
        Serial.print(F("Saved to Slot "));
        Serial.print(slot + 1);
        Serial.print(F(" ("));
        Serial.print(getSlotNoteCount(slot));
        Serial.println(F(" notes)"));
      }
      Serial.println();
      return MODE_FREE_PLAY;
    } else {
      Serial.println(F("\nNot currently recording."));
    }
  }

  // ---- PLAYBACK COMMANDS ----
  else if (input == 'P') {
    if (Serial.available() > 0) {
      char slot_char = Serial.read();

      // Play all slots
      if (slot_char == 'A' || slot_char == 'a') {
        if (playAllSlots(current_overlap_strategy)) {
          Serial.print(F("\nPlaying all slots ("));
          printOverlapStrategy(current_overlap_strategy);
          Serial.println(F(" mode)..."));
          return MODE_PLAYBACK;
        } else {
          Serial.println(F("\nNo recordings to play."));
        }
      }
      // Play specific slot
      else if (slot_char >= '1' && slot_char <= '0' + NUM_RECORDING_SLOTS) {
        int slot_num = slot_char - '1';
        if (playSingleSlot(slot_num)) {
          Serial.print(F("\nPlaying Slot "));
          Serial.print(slot_num + 1);
          Serial.println(F("..."));
          return MODE_PLAYBACK;
        } else {
          Serial.print(F("\nSlot "));
          Serial.print(slot_num + 1);
          Serial.println(F(" is empty."));
        }
      }
    } else {
      Serial.println(F("\nUsage: P[1-4] or PA (e.g., P1, P2, PA)"));
    }
  }

  else if (input == 'X') {
    stopPlayback();
    Serial.println(F("\nPlayback stopped."));
    return MODE_FREE_PLAY;
  }

  // ---- MANAGEMENT COMMANDS ----
  else if (input == 'L') {
    listRecordings();
  }

  else if (input == 'C') {
    if (Serial.available() > 0) {
      char slot_char = Serial.read();

      // Clear all slots
      if (slot_char == 'A' || slot_char == 'a') {
        clearAllRecordings();
        Serial.println(F("\nAll recordings cleared."));
      }
      // Clear specific slot
      else if (slot_char >= '1' && slot_char <= '0' + NUM_RECORDING_SLOTS) {
        int slot_num = slot_char - '1';
        if (clearRecordingSlot(slot_num)) {
          Serial.print(F("\nSlot "));
          Serial.print(slot_num + 1);
          Serial.println(F(" cleared."));
        }
      }
    } else {
      Serial.println(F("\nUsage: C[1-4] or CA (e.g., C1, C2, CA)"));
    }
  }

  // ---- OVERLAP MODE SELECTION ----
  else if (input == 'M') {
    if (Serial.available() > 0) {
      char mode_char = Serial.read();

      if (mode_char >= '1' && mode_char <= '4') {
        current_overlap_strategy = (OverlapStrategy)(mode_char - '1');
        Serial.print(F("\nOverlap mode set to: "));
        printOverlapStrategy(current_overlap_strategy);
        Serial.println();
      } else {
        Serial.println(F("\nUsage: M[1-4] (M1=High, M2=Low, M3=Alternate, M4=Drop)"));
      }
    }
  }

  // ---- HELP / INVALID COMMAND ----
  else if (input == 'H' || input == '?') {
    printMainMenu();
  }

  return current_mode;  // Return current mode if no change
}

/**
 * Check and process serial input
 * @return Updated system mode
 */
SystemMode processSerialInput() {
  if (Serial.available() > 0) {
    char input = Serial.read();

    // Ignore newlines and carriage returns
    if (input == '\n' || input == '\r') {
      return current_mode;
    }

    return handleSerialCommand(input);
  }

  return current_mode;
}

#endif // UI_H
