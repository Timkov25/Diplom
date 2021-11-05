#include <MozziGuts.h>
#include <Oscil.h> // oscillator 
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <Smooth.h>
#include <AutoMap.h> // maps unpredictable inputs to a range

// desired carrier frequency max and min, for AutoMap
const int MIN_CARRIER_FREQ = 22;
const int MAX_CARRIER_FREQ = 5000;

// desired intensity max and min, for AutoMap, note they're inverted for reverse dynamics
const int MIN_INTENSITY = 700;
const int MAX_INTENSITY = 10;

// desired mod speed max and min, for AutoMap, note they're inverted for reverse dynamics
const int MIN_MOD_SPEED = 10000;
const int MAX_MOD_SPEED = 1;

//desired values for two new parameters
const int MIN = 1;
const int MAX = 10;

AutoMap kMapCarrierFreq(0, 1023, MIN_CARRIER_FREQ, MAX_CARRIER_FREQ);
AutoMap kMapIntensity(0, 1023, MIN_INTENSITY, MAX_INTENSITY);
AutoMap kMapModSpeed(0, 1023, MIN_MOD_SPEED, MAX_MOD_SPEED);
AutoMap mapNewParameters(0, 1023, MIN, MAX);

const int KNOB_PIN_0 = 0; // set the input for the knob to analog pin 0
const int KNOB_PIN_1 = 1; // set the analog input for fm_intensity to pin 1
const int KNOB_PIN_2 = 2; // set the analog input for mod rate to pin 2
const int KNOB_PIN_3 = 3;
const int KNOB_PIN_4 = 4;

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kIntensityMod(COS2048_DATA);

int mod_ratio = 5; // brightness (harmonics)
long fm_intensity; // carries control info from updateControl to updateAudio

// smoothing for intensity to remove clicks on transitions
float smoothness = 0.95f;
Smooth <long> aSmoothIntensity(smoothness);

void setup() {
  Serial.begin(115200); // set up the Serial output so we can look at the light level
  startMozzi(); // :))
}

void updateControl() {
  // read the knob
  int knob_0_value = mozziAnalogRead(KNOB_PIN_0); // value is 0-1023

  // map the knob to carrier frequency
  int carrier_freq = kMapCarrierFreq(knob_0_value);
  int freqVal = mozziAnalogRead(KNOB_PIN_3); // value is 0-1023
  int FRQ = mapNewParameters(freqVal);
  
  //calculate the modulation frequency to stay in ratio
  int mod_freq = carrier_freq * mod_ratio * FRQ;

  // set the FM oscillator frequencies
  aCarrier.setFreq(carrier_freq);
  aModulator.setFreq(mod_freq);

  int knob4 = mozziAnalogRead(KNOB_PIN_4); // value is 0-1023
  int knob4Val = mapNewParameters(knob4);

  // read the light dependent resistor on the width Analog input pin
  int KNOB_PIN_1_value = mozziAnalogRead(KNOB_PIN_1); // value is 0-1023
  // print the value to the Serial monitor for debugging
  Serial.print("KNOB_PIN_1 = ");
  Serial.print(KNOB_PIN_1_value);
  Serial.print("\t"); // prints a tab

  int KNOB_PIN_1_calibrated = kMapIntensity(KNOB_PIN_1_value);
  Serial.print("KNOB_PIN_1_calibrated = ");
  Serial.print(KNOB_PIN_1_calibrated);
  Serial.print("\t"); // prints a tab

  // calculate the fm_intensity
  fm_intensity = ((long)KNOB_PIN_1_calibrated * knob4Val * (kIntensityMod.next() + 128)) >> 8; // shift back to range after 8 bit multiply
  Serial.print("fm_intensity = ");
  Serial.print(fm_intensity);
  Serial.print("\t"); // prints a tab

  // read the light dependent resistor on the speed Analog input pin
  int KNOB_PIN_2_value = mozziAnalogRead(KNOB_PIN_2); // value is 0-1023
  Serial.print("KNOB_PIN_2 = ");
  Serial.print(KNOB_PIN_2_value);
  Serial.print("\t"); // prints a tab

  // use a float here for low frequencies
  float mod_speed = (float)kMapModSpeed(KNOB_PIN_2_value) / 1000;
  Serial.print("   mod_speed = ");
  Serial.print(mod_speed);
  kIntensityMod.setFreq(mod_speed);

  Serial.println(); // finally, print a carraige return for the next line of debugging info
}

int updateAudio() {
  long modulation = aSmoothIntensity.next(fm_intensity) * aModulator.next();
  return aCarrier.phMod(modulation);
}

void loop() {
  audioHook();
}
