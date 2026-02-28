#include "MCP23017.h"

/*pin definitions 
  choose sda and scl pins based on pin mapping of
  the board selected
*/
#define I2C_SDA 4
#define I2C_SCL 5

MCP23017 mcp1(I2C_SDA, I2C_SCL);  //create mcp instance

#define MIDI_NOTE_F3 53
#define MIDI_NOTE_C2 36

int value, b, i, j, key, note;

void noteOn(int cmd, int pitch, int velocity, int i, int j);


class MCP23017Keyboard
{
  private:
  int key_states[4][16];
  unsigned long key_change_ts[4][16];
  int key_state = 0;  
  int keyb_map[16];
  int *pin_map;
  int octaves;
  int keys;
  int lowest_key;
  int address;
  MCP23017 *mcp;
  int output_pins[4];
  int note_map[16];
  int midi_channel;

  public:

  MCP23017Keyboard(MCP23017 *mcp, int address, int octaves, int keys, int lowest_key, int *pin_map, int midi_channel);
  void Init(void);
  void poll_status(void);
};

MCP23017Keyboard::MCP23017Keyboard(MCP23017 *mcp, int address, int octaves, int keys, int lowest_key, int *pin_map, int midi_channel) 
{
  this->mcp = mcp;
  this->address = address;
  this->octaves = octaves;
  this->keys = keys;
  this->lowest_key = lowest_key;
  this->pin_map = pin_map;
  this->midi_channel = midi_channel;
}

void MCP23017Keyboard::Init(void)
{
  int i, j, input;
  unsigned long current_ts;
  current_ts = micros();

  for (i = 0; i < 16; i++)
  {
    if (this->pin_map[i] >= 0)
    {
      if (this->pin_map[i] != 999)
        this->note_map[this->pin_map[i]] = i;
    }
    else
      this->output_pins[-this->pin_map[i] - 1] = i;
  }

  Serial.print("Note map: ");
  for (i = 0; i < this->keys; i++) 
  {
    Serial.print(this->note_map[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("Output pins: ");
  for (i = 0; i < this->octaves; i++) 
  {
    Serial.print(this->output_pins[i]);
    Serial.print(" ");
  }
  Serial.println();


  for (j = 0; j < 4; j++) 
  {
    for (i = 0; i < this->keys; i++) 
    {
      this->key_states[j][i] = 0;
      this->key_change_ts[j][i] = current_ts;
    }
  }

  input = 0;
  for (i = 0; i < 8; i++) 
  {
    if (this->pin_map[i] >= 0)
      input = input | (1 << i);
    else
      this->output_pins[1 - pin_map[i]] = i;
  }
  Serial.print("PORT A Input mask: ");
  Serial.println(input, BIN);
  this->mcp->iodir(MCP23017_PORTA, input, this->address);

  input = 0;
  for (i = 0; i < 8; i++) 
  {
    if (this->pin_map[i + 8] >= 0)
      input = input | (1 << i);
    else
      this->output_pins[-pin_map[i + 8]] = i + 8;
  }
  Serial.print("PORT B Input mask: ");
  Serial.println(input, BIN);
  this->mcp->iodir(MCP23017_PORTB, input, address);
  this->mcp->write_gpio(MCP23017_PORTA, 0x00, this->address);
  this->mcp->write_gpio(MCP23017_PORTB, 0x00, this->address);
}

int time_threshold = 2000;
// int keyb_map[12] = {12,11,10,9,7,8,6,5,1,2,3,4};

void MCP23017Keyboard::poll_status(void)
{
  int i, j, mask, key_state, value, note;
  unsigned long current_ts;

  for (int j = 0; j < this->octaves; j++)
  {
    if (this->output_pins[j] < 8)
      mcp1.write_gpio(MCP23017_PORTA, 0x1 << this->output_pins[j], this->address);
    else
      mcp1.write_gpio(MCP23017_PORTB, 0x1 << (this->output_pins[j] - 8), this->address);
    delay(10);
//    Serial.println(0x1 << this->output_pins[j]);


    value = mcp1.read_gpio(MCP23017_PORTA, this->address) | (mcp1.read_gpio(MCP23017_PORTB, this->address) << 8);
//    Serial.println(value);

    for (int i = 0; i < this->keys; i++) {
      mask = 1 << this->note_map[i];
      if (value & mask) {
        key_state = 1;
      } else {
        key_state = 0;
      }
      current_ts = micros();
      note = this->lowest_key + j * 12 + i;

      if ((key_states[j][i] == 0) && (key_state > 0) && (current_ts - key_change_ts[j][i] > time_threshold))
      {
        noteOn(0x90 + this->midi_channel, note, 0x45, i, j);
        key_states[j][i] = key_state;
        key_change_ts[j][i] = current_ts;
      } else if ((key_states[j][i] > 0) && (key_state == 0) && (current_ts - key_change_ts[j][i] > time_threshold)) {
        noteOn(0x90 + this->midi_channel, note, 0x00, i, j);
        key_states[j][i] = key_state;
        key_change_ts[j][i] = current_ts;
      }
    }
  }
}

void noteOn(int cmd, int pitch, int velocity, int i, int j) 
{
  if (velocity == 0) 
  {
    Serial.print("Note ");
    Serial.print(pitch);
    Serial.print(" off");
    Serial.println("");
  }
  else 
  {
    Serial.print("Note ");
    Serial.print(pitch);
    Serial.print(" on ");
    Serial.print(" ");
    Serial.print(i);
    Serial.print(" ");
    Serial.print(j);
    Serial.println("");
  }
/*  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity); */
}

MCP23017Keyboard *pedals;

void setup() {
  Serial.begin(74880);
  int pedal_pinmap[] = {-1, 999, 999, 0, 1, 2, 3, 4,   12,11,10,9,8,7,6,5};
  pedals = new MCP23017Keyboard(&mcp1, MCP23017_ADDRESS_22, 1, 13, MIDI_NOTE_C2, pedal_pinmap, 0);
  Serial.println("Initializing");
  pedals->Init();
  Serial.println("OK!");
}

void loop() {
  pedals->poll_status();
}
