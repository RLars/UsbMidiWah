#include <DigiMIDI.h>
#include <math.h>

/* SensorPin P2 number 1!!! */
int sensorPin = 1;  
/* SwitchPin P0 number 0!!! */  
int switchPin = 0;
#define SWITCH_DEBOUNCE_DELAY 50

#define THRESH_HIGH 995

DigiMIDIDevice midi;
#define MIDI_CHANNEL          1
#define COMMAND_EXPRESSION    0x0B /* See https://www.midi.org/specifications/item/table-3-control-change-messages-data-bytes-2. You can use other commands. */
#define MIDI_MIN_EXPRESSION   0
#define MIDI_MAX_EXPRESSION   127
#define ANALOG_MIN_VAL        0
#define ANALOG_MAX_VAL        1023
#define COMMAND_ON_OFF        0x43 /* See https://www.midi.org/specifications/item/table-3-control-change-messages-data-bytes-2. You can use other commands. */
#define MIDI_OFF_VAL          0 
#define MIDI_ON_VAL           127

void processSensorValue(int sensorValue) {
  /* Use impossible initial old value. */
  static uint8_t oldExpVal = 255;

  //int linVal = log(sensorValue) / log(1000) * 1023;
  uint8_t expVal = map(sensorValue, 0, 1023, MIDI_MIN_EXPRESSION, MIDI_MAX_EXPRESSION);
  

  /* Set expression value if changed */
  if (oldExpVal != expVal) {    
    midi.sendControlChange(COMMAND_EXPRESSION, expVal, MIDI_CHANNEL);
    oldExpVal = expVal;
  }
}

void onButtonStateChange(int state) {
  if (HIGH == state) {
    midi.sendControlChange(COMMAND_ON_OFF, MIDI_ON_VAL, MIDI_CHANNEL);
  } else {
    midi.sendControlChange(COMMAND_ON_OFF, MIDI_OFF_VAL, MIDI_CHANNEL);
  }
}

void debounceSwitch(int reading) {
  /* Initialize with impossible value, so initial value will be sensed as state change. */
  static int buttonState = -1;
  static int lastButtonState = -1;
  
  static unsigned long lastDebounceTime = 0;

  const unsigned long debounceDelay = SWITCH_DEBOUNCE_DELAY;    
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    lastButtonState = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      onButtonStateChange(buttonState);
    }
  }
}

void setup() {
  /* Declare switchPin as input */
  pinMode(switchPin, INPUT_PULLUP);
}

void loop() {
  debounceSwitch(digitalRead(switchPin));
  processSensorValue(analogRead(sensorPin));
}
