#include "MCP23017.h"

/*pin definitions 
  choose sda and scl pins based on pin mapping of
  the board selected
*/
#define I2C_SDA 4
#define I2C_SCL 5

MCP23017 mcp1(I2C_SDA, I2C_SCL);  //create mcp instance

#define MIDI_NOTE_F3 53

int value, b, i, j, key, note;


unsigned long current_ts;
int key_states[4][12];
unsigned long key_change_ts[4][12];
int key_state = 0;  
int time_threshold = 2000;

int keyboard[4][12];

int keyb_map[12] = {12,11,10,9,7,8,6,5,1,2,3,4};

void setup() {
  Serial.begin(74880);
  // put your setup code here, to run once:

  current_ts = micros();
  for (j = 0; j < 4; j++) {
    for (i = 0; i < 12; i++) {
      key_states[j][i] = 0;
      key_change_ts[j][i] = current_ts;
    }
  }

  /*set i/o pin direction as output for both ports A and B*/
  mcp1.iodir(MCP23017_PORTA, MCP23017_IODIR_ALL_INPUT, MCP23017_ADDRESS_22);
  mcp1.iodir(MCP23017_PORTB, 
    (MCP23017_IODIR_IO0_INPUT
    | MCP23017_IODIR_IO1_INPUT
    | MCP23017_IODIR_IO2_INPUT
    | MCP23017_IODIR_IO3_INPUT)
  , MCP23017_ADDRESS_20);
//  mcp1.gppu(MCP23017_PORTA, MCP23017_GPPU_ALL_ENABLED, MCP23017_ADDRESS_20);
//  mcp1.gppu(MCP23017_PORTB, MCP23017_GPPU_ALL_ENABLED, MCP23017_ADDRESS_20);
  mcp1.write_gpio(MCP23017_PORTA, 0x00, MCP23017_ADDRESS_22);
  mcp1.write_gpio(MCP23017_PORTB, 0x40, MCP23017_ADDRESS_22);
}

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int j = 0; j < 4; j++) { 
    mcp1.write_gpio(MCP23017_PORTB, 0x10 << j, MCP23017_ADDRESS_22);
    delay(10);

    /* set gpio port A and B output as high */
    value = mcp1.read_gpio(MCP23017_PORTA, MCP23017_ADDRESS_22) | (mcp1.read_gpio(MCP23017_PORTB, MCP23017_ADDRESS_22) << 8);
    b = 1;
    key = 0;

    for (int i = 0; i < 12; i++) {
      if (value & b) {
        key_state = 1;
      } else {
        key_state = 0;
      }

      current_ts = micros();
//      note = MIDI_NOTE_F3 + j * 12 + keyb_map[(i + 5) % 12];
      note = MIDI_NOTE_F3 + j * 12 + keyb_map[i];

      if ((key_states[j][i] == 0) && (key_state > 0) && (current_ts - key_change_ts[j][i] > time_threshold)) {
        // Note On
        // BLEMidiServer.noteOn(0, note, 127);

        Serial.print("Note ");
        Serial.print(note);
        Serial.print(" on ");
        Serial.print(i);
        Serial.print(" -> ");
        Serial.print(keyb_map[i]);
        Serial.print(" ");
        Serial.println(j); 
//        noteOn(0x90, note, 0x45);
        key_states[j][i] = key_state;
        key_change_ts[j][i] = current_ts;
        

      } else if ((key_states[j][i] > 0) && (key_state == 0) && (current_ts - key_change_ts[j][i] > time_threshold)) {
        // Note Off
        // BLEMidiServer.noteOff(0, note, 127);
        Serial.print("Note ");
        Serial.print(note);
        Serial.println(" off"); 
        noteOn(0x90, note, 0x00);
        key_states[j][i] = key_state;
        key_change_ts[j][i] = current_ts;
      }


      b = b << 1;
      key++;
    }
  }

  // delay(10);
}
