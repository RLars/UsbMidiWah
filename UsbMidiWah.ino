#include "Input.hpp"

/* ======================================= CONFIG ====================================== */
/* Add new Analog or Digital input objects here and add them to the inputs array.        */
/* For a list of midi commands see:                                                      */
/* https://www.midi.org/specifications/item/table-3-control-change-messages-data-bytes-2 */
/* ===================================================================================== */

/* Pin P2 = 1 for analog readings, Midi command "Expression Controller" = 0x0B */
static AnalogInput poti(1, 0x0B);
/* Pin P0 = 0, Use internal Pullup resistor, DebounceDelay = 50ms, Midi command "Soft Pedal On/Off" = 0x43 */
static DigitalInput onOffSwitch(0, INPUT_PULLUP, 50, 0x43);

Input * const inputs [] = {
  (Input*)&poti,
  (Input*)&onOffSwitch
};

void setup() {
  /* Iterate over list of inputs and initialize */
  for( unsigned int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++ ) {
    inputs[i]->initialize();
  }
}

void loop() {
  /* Iterate over list of inputs and process */
  for( unsigned int i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++ ) {
    inputs[i]->process();
  }
}
