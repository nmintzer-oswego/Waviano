#define ECHO_PIN 3       
#define TRIGGER_PIN 4      

#define LED_Do 13
#define LED_Re 12
#define LED_Mi 11 
#define LED_Fa 10
#define LED_Sol 9 
#define LED_La 8 
#define LED_Si 7  
#define LED_Do_High 6 

#define BUZZER_PIN 2

unsigned long last_time_ultrasonic_trigger = millis(); 
unsigned long ultrasonic_trigger_delay = 100; 

volatile unsigned long pulse_in_begin;
volatile unsigned long pulse_in_end;
volatile bool new_distance_available = false;

const int time_delay = 500;

// 🎵 Notes (8 notes: Do Re Mi Fa Sol La Si Do)
int notes[] = {
  523,  // Do (C5)
  587,  // Re (D5)
  659,  // Mi (E5)
  698,  // Fa (F5)
  784,  // Sol (G5)
  880,  // La (A5)
  988,  // Si (B5)
  1046  // Do (C6) 
};

// 🎵 SONGS
int melody_mary[] = {
  LED_Mi, LED_Re, LED_Do, LED_Re, LED_Mi, LED_Mi, LED_Mi,
  LED_Re, LED_Re, LED_Re, LED_Mi, LED_Sol, LED_Sol, LED_Mi, LED_Re,
  LED_Do, LED_Re, LED_Mi, LED_Mi, LED_Mi, LED_Re, LED_Re,
  LED_Mi, LED_Re, LED_Do
};
int length_mary = sizeof(melody_mary) / sizeof(melody_mary[0]);

int melody_twinkle[] = {
  LED_Do, LED_Do, LED_Sol, LED_Sol, LED_La, LED_La, LED_Sol,
  LED_Fa, LED_Fa, LED_Mi, LED_Mi, LED_Re, LED_Re, LED_Do,
  LED_Sol, LED_Sol, LED_Fa, LED_Fa, LED_Mi, LED_Mi, LED_Re,
  LED_Sol, LED_Sol, LED_Fa, LED_Fa, LED_Mi, LED_Mi, LED_Re,
  LED_Do, LED_Do, LED_Sol, LED_Sol, LED_La, LED_La, LED_Sol,
  LED_Fa, LED_Fa, LED_Mi, LED_Mi, LED_Re, LED_Re, LED_Do
};
int length_twinkle = sizeof(melody_twinkle) / sizeof(melody_twinkle[0]);

int melody_bus[] = {
  LED_Do, LED_Do, LED_Do, LED_Mi, LED_Sol,
  LED_Mi, LED_Do, LED_Re, LED_Do, LED_Re,
  LED_Sol, LED_Mi, LED_Do, LED_Do, LED_Do,
  LED_Do, LED_Mi, LED_Sol, LED_Mi, LED_Do,
  LED_Re, LED_Sol, LED_Do, LED_Do
};
int length_bus = sizeof(melody_bus) / sizeof(melody_bus[0]);

int melody_yankee_doodle[] = {
  LED_Fa, LED_Fa, LED_Sol, LED_La, LED_Fa, LED_La, LED_Sol,
  LED_Fa, LED_Fa, LED_Sol, LED_La, LED_Fa, LED_Mi,
  LED_Fa, LED_Fa, LED_Sol, LED_La, LED_Si, LED_La, LED_Sol,
  LED_Fa, LED_Mi, LED_Do, LED_Re, LED_Mi, LED_Fa, LED_Fa
};
int length_yankee_doodle = sizeof(melody_yankee_doodle) / sizeof(melody_yankee_doodle[0]);

int* current_melody = melody_mary;
int current_melody_length = length_mary;

int melody_tracker = 0;
bool free_play_mode = false;

void trigger_ultrasonic_sensor() {
  digitalWrite(TRIGGER_PIN, LOW);  
  delayMicroseconds(2);             
  digitalWrite(TRIGGER_PIN, HIGH);  
  delayMicroseconds(10);            
  digitalWrite(TRIGGER_PIN, LOW);   
}

void echo_pin_interrupt() {
  if (digitalRead(ECHO_PIN) == HIGH) {
    pulse_in_begin = micros();
  } else {
    pulse_in_end = micros();
    new_distance_available = true;
  }
}

double get_distance() {
  double duration_micros = pulse_in_end - pulse_in_begin;
  return duration_micros / 58.0;
}

void turn_off_all_leds() {
  digitalWrite(LED_Do, LOW);
  digitalWrite(LED_Re, LOW);
  digitalWrite(LED_Mi, LOW);
  digitalWrite(LED_Fa, LOW);
  digitalWrite(LED_Sol, LOW);
  digitalWrite(LED_La, LOW);
  digitalWrite(LED_Si, LOW);
  digitalWrite(LED_Do_High, LOW);  // Turn off high Do
}

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);   
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(LED_Do, OUTPUT);
  pinMode(LED_Re, OUTPUT);
  pinMode(LED_Mi, OUTPUT);
  pinMode(LED_Fa, OUTPUT);
  pinMode(LED_Sol, OUTPUT);
  pinMode(LED_La, OUTPUT);
  pinMode(LED_Si, OUTPUT);
  pinMode(LED_Do_High, OUTPUT);  
  pinMode(BUZZER_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echo_pin_interrupt, CHANGE);

  Serial.println("🎶 Select a song to play:");
  Serial.println("1 - Mary Had a Little Lamb");
  Serial.println("2 - Twinkle Twinkle Little Star");
  Serial.println("3 - The Wheels on the Bus");
  Serial.println("4 - Yankee Doodle");
  Serial.println("0 - Free play mode (Air Piano)");
}

void loop() {
  unsigned long time_now = millis();

  if (Serial.available() > 0) {
    char input = Serial.read();
    melody_tracker = 0;

    if (input == '1') {
      current_melody = melody_mary;
      current_melody_length = length_mary;
      free_play_mode = false;
      Serial.println("Now playing: Mary Had a Little Lamb");
    } else if (input == '2') {
      current_melody = melody_twinkle;
      current_melody_length = length_twinkle;
      free_play_mode = false;
      Serial.println("Now playing: Twinkle Twinkle Little Star");
    } else if (input == '3') {
      current_melody = melody_bus;
      current_melody_length = length_bus;
      free_play_mode = false;
      Serial.println("Now playing: The Wheels on the Bus");
    } else if (input == '4') {
      current_melody = melody_yankee_doodle;
      current_melody_length = length_yankee_doodle;
      free_play_mode = false;
      Serial.println("Now playing: Yankee Doodle");
    } else if (input == '0') {
      free_play_mode = true;
      Serial.println("🎹 Free play mode activated!");
    } else {
      Serial.println("Invalid choice. Please enter 0, 1, 2, 3, or 4.");
    }
  }

  if (time_now - last_time_ultrasonic_trigger > ultrasonic_trigger_delay) {
    last_time_ultrasonic_trigger = time_now;
    trigger_ultrasonic_sensor();
  }

  if (!free_play_mode && melody_tracker < current_melody_length) {
    int next_led = current_melody[melody_tracker];
    turn_off_all_leds();
    digitalWrite(next_led, HIGH);
  }

  if (new_distance_available) {
    new_distance_available = false;
    double distance = get_distance();
    int note_index = -1;

    if (distance > 2 && distance <= 10) note_index = 0;
    else if (distance > 10 && distance <= 20) note_index = 1;
    else if (distance > 20 && distance <= 30) note_index = 2;
    else if (distance > 30 && distance <= 40) note_index = 3;
    else if (distance > 40 && distance <= 50) note_index = 4;
    else if (distance > 50 && distance <= 60) note_index = 5;
    else if (distance > 60 && distance <= 70) note_index = 6;
    else if (distance > 70 && distance <= 80) note_index = 7;  // NEW: High Do

    if (note_index != -1) {
      int led;
      if (note_index == 7) {
        led = LED_Do_High;  // NEW High Do
      } else {
        led = LED_Do - note_index;
      }

      if (free_play_mode) {
        turn_off_all_leds();
        digitalWrite(led, HIGH);
        tone(BUZZER_PIN, notes[note_index]);
        delay(time_delay);
        noTone(BUZZER_PIN);
      } else if (melody_tracker < current_melody_length && current_melody[melody_tracker] == led) {
        tone(BUZZER_PIN, notes[note_index]);
        delay(time_delay);
        noTone(BUZZER_PIN);
        melody_tracker++;

        if (melody_tracker >= current_melody_length) {
          melody_tracker = 0;
          turn_off_all_leds();
          Serial.println("🎵 Song finished. Pick a new one (0, 1, 2, 3, or 4).\n");
        }
      }
    }
  }
}
