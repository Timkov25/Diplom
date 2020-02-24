#include <synth.h>
#include <tables.h>
#include <Keypad.h>

//=====================================================
//Keypad
//=====================================================

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'},
  {'9', '0', 'A', 'B'},
  {'C', 'D', 'E', 'F'}
};
byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


//=====================================================
//Synth
//=====================================================
synth mySynth;    //-Make a synth


int form_signal = 0;
void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  //Keypad
  customKeypad.addEventListener(keypadEvent); // Add an event listener for this keypad

  //Synth
  mySynth.begin(); // chA -- 11 pin
  mySynth.setupVoice(0, form_signal, 60, ENVELOPE0, 100, 64);
  mySynth.setupVoice(1, form_signal, 60, ENVELOPE0, 100, 64);
  mySynth.setupVoice(2, form_signal, 60, ENVELOPE0, 100, 64);
  mySynth.setupVoice(3, form_signal, 60, ENVELOPE0, 100, 64);


//  Serial.println("Getkey");
//  Serial.println(customKeypad.getKey());




}

void loop() {
//  test
  Serial.println("Getstate");
  Serial.println(customKeypad.getState());

  //  char CustomKey = customKeypad.getKey();
  //  if (CustomKey) {
  //    Serial.println(CustomKey);
  //  }
  flag_check();
}



bool flag440 = 0;
bool flag600 = 0;

void keypadEvent(KeypadEvent CustomKey) {
  //+++++++++++++++++Отладка++++++++++++++++
  //  Serial.println("Обработчик Event");

  switch (customKeypad.getState()) {
    case PRESSED:
      if (CustomKey == '1') {
        Serial.println("Кнопка один нажата");
        digitalWrite(13, 1);
      }
      if (CustomKey == '2') {
        flag440 = 1;
      }
      if (CustomKey == '3') {
        flag600 = 1;
      }
      if (CustomKey == 'F') {
        form_signal_increment();
      }


      break;

    case RELEASED:
      if (CustomKey == '1') {
        digitalWrite(13, 0);
        Serial.println("RELEASED");
      }
      if (CustomKey == '2') {
        flag440 = 0;
      }
      if (CustomKey == '3') {
        flag600 = 0;

      }
      break;

    case HOLD:
      if (CustomKey == '1') {
        while (1) {
          Serial.println("HOLD");
        }
      }
      break;
  }
}



void play(int freq)
{
  static byte voice = 0;
  static int previous_freq;

  mySynth.setFrequency(voice, freq);
  mySynth.trigger(voice);


  if (previous_freq != freq) {
    voice++;
  }

  previous_freq = freq;

  if (voice == 4)
  {
    voice = 0;
  }
}


void form_signal_increment() {
  form_signal++;
  mySynth.setWave(0, form_signal);
  mySynth.setWave(1, form_signal);
  mySynth.setWave(2, form_signal);
  mySynth.setWave(3, form_signal);
  if (form_signal == 4) {
    form_signal = 0;
  }
}


void flag_check() {
  if (flag440) {
    play(440);
  }
  if (flag600) {
    play(600);

  }
}
