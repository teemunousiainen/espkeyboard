/*
  
*/

// constants won't change. They're used here to set pin numbers:
                          // D  H   C   A.  A#. G#. G.  E.  F#. D#. F.  C#
//const int buttonPins[12] = {12, 39, 36, 35, 34, 33, 32, 26, 25, 14, 27, 13};  // the number of the pushbutton pin

// const int buttonPins[12] = {36, 13, 12, 14, 26, 27, 25, 32, 33, 35, 34, 39};  // Starting from C
//const int buttonPins[12] =  {27, 25, 32, 33, 35, 34, 39, 36, 13, 12, 14, 26};  // Starting from F
const int buttonPins[12] = {13, 12, 14, 26, 27, 25, 32, 33, 35, 34, 39, 36};  // Starting from C#


// const int buttonPins[12] = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13};  // the number of the pushbutton pin
const int outputPins[4] = {17, 5, 18, 19};  // the number of the pushbutton pin
const int ledPin = 13;    // the number of the LED pin
unsigned long current_ts;

// variables will change:

int key_states[4][12];
unsigned long key_change_ts[4][12];
int key_state = 0;  
int i, j;
int note;

int time_threshold = 2000;

void setup() {
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
  Serial.begin(9600);
}

void loop() {
  // read the state of the pushbutton value:
  for (j = 0; j < 4; j++) {
    digitalWrite(outputPins[j], HIGH);
    for (i = 0; i < 12; i++) {
      key_state = analogRead(buttonPins[i]);
      current_ts = micros();

      note = 53 + j * 12 + i;
      if ((key_states[j][i] == 0) && (key_state > 0) && (current_ts - key_change_ts[j][i] > time_threshold)) {
        // Note On
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
