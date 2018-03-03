#include "Input.hpp"
#include "Arduino.h"
#include <DigiMIDI.h>

DigiMIDIDevice midi;

/* Constants */
#define MIDI_CHANNEL          1
#define MIDI_MIN_EXPRESSION   0
#define MIDI_MAX_EXPRESSION   127
#define ANALOG_MIN_VAL        0
#define ANALOG_MAX_VAL        1023
#define MIDI_OFF_VAL          0 
#define MIDI_ON_VAL           127

AnalogInput::AnalogInput(int pin, uint8_t midiCommandId) {
  mPin = pin;
  mMidiCommandId = midiCommandId;
}

void AnalogInput::process() {
  int reading = analogRead(mPin);
  /* TODO: Filter analog signal here? */
  bool stateChanged = processSensorValue(reading);
  if (stateChanged) {
    sendSensorValue();
  }
}

bool AnalogInput::processSensorValue(int sensorValue) {
  bool sensorValueChanged = false;
  
  /* Use impossible initial old value. */
  static uint8_t oldExpVal = 255;

  /* TODO: Linearize signal here? */
  sensorValue = 1023 * pow(ADC/1023.,10);

  /* Map signal to Midi value range*/
  uint8_t sensorVal = map(sensorValue, 0, 1023, MIDI_MIN_EXPRESSION, MIDI_MAX_EXPRESSION);

  /* check if value has changed */
  if (mSensorValue != sensorVal) {    
    mSensorValue = sensorVal;

    sensorValueChanged = true;
  }

  return sensorValueChanged;
}

void AnalogInput::sendSensorValue() {
  midi.sendControlChange(mMidiCommandId, mSensorValue, MIDI_CHANNEL);
}

DigitalInput::DigitalInput(uint8_t pin, uint8_t pMode, int debounceDelay, uint8_t midiCommandId) {
  mPin = pin;
  mPinMode = pMode;
  
  mDebounceDelay = debounceDelay;
  mMidiCommandId = midiCommandId;
  
  /* Initialize state variables to impossible value, so first transition is triggered automatically at startup. */
  mButtonState = -1;
  mLastButtonState = -1;
  mLastDebounceTime = 0;
}

void DigitalInput::initialize() {
  /* Declare switchPin as input */
  pinMode(mPin, mPinMode);
}

void DigitalInput::process() {
  int reading = digitalRead(mPin);
  bool stateChanged = debounce(reading);
  if (stateChanged) {
    sendButtonState();
  }
}

void DigitalInput::sendButtonState() {
  if (HIGH == mButtonState) {
    midi.sendControlChange(mMidiCommandId, MIDI_ON_VAL, MIDI_CHANNEL);
  } else {
    midi.sendControlChange(mMidiCommandId, MIDI_OFF_VAL, MIDI_CHANNEL);
  }
}

bool DigitalInput::debounce(int reading) {
  bool stateChanged = false;
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != mLastButtonState) {
    // reset the debouncing timer
    mLastDebounceTime = millis();
    mLastButtonState = reading;
  }

  if ((millis() - mLastDebounceTime) > mDebounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != mButtonState) {
      mButtonState = reading;

      stateChanged = true;
    }
  }

  return stateChanged;
}
