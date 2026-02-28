
#include <Arduino.h>
#include <BLEMidi.h>

/*
  
*/


#define MIDI_NOTE_F3 53

const int buttonPins[12] = {13, 12, 14, 27, 26, 25, 32, 33, 35, 34, 39, 36};  // Starting from C#
const int outputPins[4] = {17, 5, 18, 19};  
const int ledPin = 13;

unsigned long current_ts;
int key_states[4][12];
unsigned long key_change_ts[4][12];
int key_state = 0;  
int i, j;
int note;

int time_threshold = 2000;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin("Basic MIDI device");
  Serial.println("Waiting for connections...");
  BLEMidiServer.enableDebugging();  // Uncomment if you want to see some debugging output from the library


  current_ts = micros();
  for (j = 0; j < 4; j++) {
    for (i = 0; i < 12; i++) {
      key_states[j][i] = 0;
      key_change_ts[j][i] = current_ts;
    }
  }

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  for (i = 0; i < 12; i++) {
    pinMode(buttonPins[i], INPUT);
  }
  for (i = 0; i < 4; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);
  }
}

void loop() {
  // read the state of the pushbutton value:
  for (j = 0; j < 4; j++) {
    digitalWrite(outputPins[j], HIGH);
    for (i = 0; i < 12; i++) {
      key_state = analogRead(buttonPins[i]);
      current_ts = micros();

      note = MIDI_NOTE_F3 + j * 12 + i;

      if ((key_states[j][i] == 0) && (key_state > 0) && (current_ts - key_change_ts[j][i] > time_threshold)) {
        // Note On
        BLEMidiServer.noteOn(0, note, 127);

        Serial.print("Note ");
        Serial.print(note);
        Serial.print(" on ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(j);
        key_states[j][i] = key_state;
        key_change_ts[j][i] = current_ts;
        

      } else if ((key_states[j][i] > 0) && (key_state == 0) && (current_ts - key_change_ts[j][i] > time_threshold)) {
        // Note Off
        BLEMidiServer.noteOff(0, note, 127);
        Serial.print("Note ");
        Serial.print(note);
        Serial.println(" off");
        key_states[j][i] = key_state;
        key_change_ts[j][i] = current_ts;
      }


    }
    digitalWrite(outputPins[j], LOW);
  }
}
