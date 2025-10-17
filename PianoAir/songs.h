#ifndef SONGS_H
#define SONGS_H

#include "config.h"
#include "note_mapping.h"

// ============================================
// PRE-PROGRAMMED SONGS
// ============================================

// Mary Had a Little Lamb
int melody_mary[] = {
  LED_Mi, LED_Re, LED_Do, LED_Re, LED_Mi, LED_Mi, LED_Mi,
  LED_Re, LED_Re, LED_Re, LED_Mi, LED_Sol, LED_Sol, LED_Mi, LED_Re,
  LED_Do, LED_Re, LED_Mi, LED_Mi, LED_Mi, LED_Re, LED_Re,
  LED_Mi, LED_Re, LED_Do
};
int length_mary = sizeof(melody_mary) / sizeof(melody_mary[0]);

// Twinkle Twinkle Little Star
int melody_twinkle[] = {
  LED_Do, LED_Do, LED_Sol, LED_Sol, LED_La, LED_La, LED_Sol,
  LED_Fa, LED_Fa, LED_Mi, LED_Mi, LED_Re, LED_Re, LED_Do,
  LED_Sol, LED_Sol, LED_Fa, LED_Fa, LED_Mi, LED_Mi, LED_Re,
  LED_Sol, LED_Sol, LED_Fa, LED_Fa, LED_Mi, LED_Mi, LED_Re,
  LED_Do, LED_Do, LED_Sol, LED_Sol, LED_La, LED_La, LED_Sol,
  LED_Fa, LED_Fa, LED_Mi, LED_Mi, LED_Re, LED_Re, LED_Do
};
int length_twinkle = sizeof(melody_twinkle) / sizeof(melody_twinkle[0]);

// The Wheels on the Bus
int melody_bus[] = {
  LED_Do, LED_Do, LED_Do, LED_Mi, LED_Sol,
  LED_Mi, LED_Do, LED_Re, LED_Do, LED_Re,
  LED_Sol, LED_Mi, LED_Do, LED_Do, LED_Do,
  LED_Do, LED_Mi, LED_Sol, LED_Mi, LED_Do,
  LED_Re, LED_Sol, LED_Do, LED_Do
};
int length_bus = sizeof(melody_bus) / sizeof(melody_bus[0]);

// Yankee Doodle
int melody_yankee_doodle[] = {
  LED_Fa, LED_Fa, LED_Sol, LED_La, LED_Fa, LED_La, LED_Sol,
  LED_Fa, LED_Fa, LED_Sol, LED_La, LED_Fa, LED_Mi,
  LED_Fa, LED_Fa, LED_Sol, LED_La, LED_Si, LED_La, LED_Sol,
  LED_Fa, LED_Mi, LED_Do, LED_Re, LED_Mi, LED_Fa, LED_Fa
};
int length_yankee_doodle = sizeof(melody_yankee_doodle) / sizeof(melody_yankee_doodle[0]);

// ============================================
// SONG METADATA
// ============================================

struct Song {
  const char* name;
  int* melody;
  int length;
};

#define NUM_SONGS 4

const Song songs[NUM_SONGS] = {
  {"Mary Had a Little Lamb", melody_mary, length_mary},
  {"Twinkle Twinkle Little Star", melody_twinkle, length_twinkle},
  {"The Wheels on the Bus", melody_bus, length_bus},
  {"Yankee Doodle", melody_yankee_doodle, length_yankee_doodle}
};

// ============================================
// GUIDED MODE STATE
// ============================================

// Currently selected song (-1 = none)
int current_song_index = -1;

// Current position in the song
int melody_tracker = 0;

// ============================================
// GUIDED MODE FUNCTIONS
// ============================================

/**
 * Select a song for guided mode
 * @param song_index Song index (0-3)
 * @return true if valid song selected
 */
bool selectSong(int song_index) {
  if (song_index >= 0 && song_index < NUM_SONGS) {
    current_song_index = song_index;
    melody_tracker = 0;
    return true;
  }
  return false;
}

/**
 * Get the current song
 * @return Pointer to current song, or NULL if none selected
 */
const Song* getCurrentSong() {
  if (current_song_index >= 0 && current_song_index < NUM_SONGS) {
    return &songs[current_song_index];
  }
  return NULL;
}

/**
 * Get the next LED to light up in guided mode
 * @return LED pin number, or -1 if song is finished
 */
int getNextGuidedLED() {
  const Song* song = getCurrentSong();
  if (song != NULL && melody_tracker < song->length) {
    return song->melody[melody_tracker];
  }
  return -1;
}

/**
 * Check if a note matches the expected note in guided mode
 * @param note_index Note index to check
 * @return true if note matches expected note
 */
bool checkGuidedNote(int note_index) {
  const Song* song = getCurrentSong();
  if (song == NULL || melody_tracker >= song->length) {
    return false;
  }

  int expected_led = song->melody[melody_tracker];
  int actual_led = getNoteLED(note_index);

  if (expected_led == actual_led) {
    melody_tracker++;
    return true;
  }
  return false;
}

/**
 * Check if guided mode song is finished
 * @return true if song is complete
 */
bool isGuidedSongFinished() {
  const Song* song = getCurrentSong();
  if (song == NULL) {
    return true;
  }
  return melody_tracker >= song->length;
}

/**
 * Reset guided mode (restart song from beginning)
 */
void resetGuidedMode() {
  melody_tracker = 0;
}

/**
 * Clear song selection (exit guided mode)
 */
void clearSongSelection() {
  current_song_index = -1;
  melody_tracker = 0;
}

/**
 * Get song progress
 * @param out_current Output: current note position
 * @param out_total Output: total notes in song
 * @return true if song is active
 */
bool getSongProgress(int* out_current, int* out_total) {
  const Song* song = getCurrentSong();
  if (song != NULL) {
    *out_current = melody_tracker;
    *out_total = song->length;
    return true;
  }
  return false;
}

#endif // SONGS_H
