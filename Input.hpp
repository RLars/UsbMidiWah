#ifndef __INPUT_HPP
#define __INPUT_HPP

#include <stdint.h>

/* Interface class Input for polymorphic handling of inputs. */
class Input {
  public:
    virtual void initialize() = 0;
    virtual void process() = 0;
};

/* Class for analog inputs, whose input is mapped to MIDI values 0-127 and output as command on change. */
class AnalogInput : public Input {
  private:
    uint8_t mPin;
    uint8_t mMidiCommandId;

    int mSensorValue;
  
    bool processSensorValue(int reading);
    void sendSensorValue();
  public:
    AnalogInput(int pin, uint8_t midiCommandId); /* Constructor */
    virtual void initialize() { /* No initialization necessary. */ }
    virtual void process();
};

/* Class for digital inputs, whose input is mapped to MIDI values 0 or 127 and output as command on change after debouncing. */
class DigitalInput : public Input {
  private:  
    uint8_t mPin;
    uint8_t mPinMode;
    int mDebounceDelay;
    uint8_t mMidiCommandId;
  
    int mButtonState;
    int mLastButtonState;
    unsigned long mLastDebounceTime;

    bool debounce(int reading);
    void sendButtonState();
  public:
    DigitalInput(uint8_t pin, uint8_t pMode, int debounceDelayMs, uint8_t midiCommandId); /* Constructor */
    virtual void initialize();
    virtual void process();
};

#endif /* __INPUT_HPP */
