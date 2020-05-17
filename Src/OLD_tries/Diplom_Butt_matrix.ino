#include <synth.h>
#include <tables.h>

unsigned long last_time_form_signal = 0;

float C2_note = 523.25;
float CD2_note = 554.36;
float D2_note = 587.32;
float D2d_note = 622.26;
float E2_note = 659.26;
float F2_note = 698.46;
float F2d_note = 739.98;
float G2_note = 784.00;
float G2d_note = 830.60;
float A2_note = 880.00;
float A2d_note = 932.32;
float B2_note = 987.75;
float C3_note = 1046.50;


float note_value_arr [4][4] =
{ {C2_note, D2_note, E2_note, F2_note},
  {G2_note, A2_note, B2_note, C3_note},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

bool button_state_arr [4][4] = {};


//=====================================================
//Synth
//=====================================================
synth mySynth;    //-Make a synth

bool flag_form_signal_increment = 0;

void setup() {
  Serial.begin(9600);

//=====================================================
//Keypad
//=====================================================
  //====Входы====
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  //====Выходы====
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  //  ====Все в 1====
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);

//=====================================================
//Synth
//=====================================================
  mySynth.begin(); // chA -- 11 pin
  mySynth.setupVoice(0, SINE, 60, ENVELOPE0, 100, 64);
  mySynth.setupVoice(1, SINE, 60, ENVELOPE0, 100, 64);
  mySynth.setupVoice(2, SINE, 60, ENVELOPE0, 100, 64);
  mySynth.setupVoice(3, SINE, 60, ENVELOPE0, 100, 64);

}
void loop() {
  Request_keypad();
  Check_button_state();
  form_signal_increment();
}


void Request_keypad() {
  //====обнуляем массив====
  memset( button_state_arr, 0, sizeof(button_state_arr));
  for (int i = 2; i <= 5; i++) {
    digitalWrite (i, LOW);
    for (int j = 6; j <= 9; j++) {

      if (digitalRead(j) == 0) {
        button_state_arr[i - 2][j - 6] = true;
      }
    }
    digitalWrite (i, HIGH);
  }
}

void Check_button_state() {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      if (button_state_arr[j][i]) {
        play(note_value_arr[i][j]);
      }
    }
  }

  if (button_state_arr[3][3]) {
    flag_form_signal_increment = true;
  }

}


void out_state_button() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      Serial.print (button_state_arr[j][i]);
      Serial.print("  ");
    }
    Serial.println();
  }
  Serial.println();
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
  if (flag_form_signal_increment && millis() - last_time_form_signal > 150) {
    static byte  form_signal = 0;
    form_signal++;
    mySynth.setWave(0, form_signal);
    mySynth.setWave(1, form_signal);
    mySynth.setWave(2, form_signal);
    mySynth.setWave(3, form_signal);
    if (form_signal == 4) {
      form_signal = 0;
    }
    flag_form_signal_increment = false;
    last_time_form_signal = millis();
  }
}
