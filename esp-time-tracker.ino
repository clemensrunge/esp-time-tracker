#include "themes.h"

const int buzzerPin = 5;

const int buttonPinRed = 2;
const int ledPinRed =  0;

const int buttonPinGreen = 16;
const int ledPinGreen =  13;

int buttonStateRed = 0;
int buttonStateGreen = 0;
int workeState = 0;

void setup(void)
{
  pinMode(buzzerPin, OUTPUT);//buzzer
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  pinMode(buttonPinRed, INPUT_PULLUP);
  pinMode(buttonPinGreen, INPUT_PULLUP);
}

void loop()
{
  buttonStateRed = digitalRead(buttonPinRed);
  buttonStateGreen = digitalRead(buttonPinGreen);

  if (buttonStateRed == LOW) {
    workeState = false;
  }

  if (buttonStateGreen == LOW) {
    workeState = true;
  }
  
  if (workeState == LOW) {
    digitalWrite(ledPinRed, HIGH);
  } else {
    digitalWrite(ledPinRed, LOW);
  }
    
  if (workeState == HIGH) {
    digitalWrite(ledPinGreen, HIGH);
  } else {
    digitalWrite(ledPinGreen, LOW);
  }

   if (buttonStateRed == LOW) {
    playTheme(underworld_melody, underworld_tempo, sizeof(underworld_melody) / sizeof(int), buzzerPin);
  }

  if (buttonStateGreen == LOW) {
    playTheme(melody, noteDurations, sizeof(melody) / sizeof(int), buzzerPin);
  }
}


void playTheme(int melody[], int noteDurations[], int length, int pin){
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < length; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buzzerPin);
  }
}
