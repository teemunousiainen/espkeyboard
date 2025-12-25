/*
  
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPins[12] = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13};  // the number of the pushbutton pin
const int outputPins[4] = {19, 18, 5, 17};  // the number of the pushbutton pin
const int ledPin = 13;    // the number of the LED pin

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int i, j;

void setup() {
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
      buttonState = analogRead(buttonPins[i]);
      Serial.print(buttonState);
      Serial.print(" ");
    }
    digitalWrite(outputPins[j], LOW);
  }

  Serial.println("");

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}
