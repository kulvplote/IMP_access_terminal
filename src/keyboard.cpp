#include "Keyboard.h"

byte rows[ROWS] = {5, 13, 12, 14};
byte columns[COLS] = {19, 18, 23};

char keyboardChars[ROWS][COLS] = {
    {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};
int debounceDelay = 300;
long lastKeyPressTime = 0;

char get_keyboard_key() {
  for (int a = 0; a < ROWS; a++) {
    digitalWrite(rows[a], HIGH);

    for (int b = 0; b < COLS; b++) {
      if (digitalRead(columns[b]) == HIGH) {
        if (millis() - lastKeyPressTime > debounceDelay) {
          lastKeyPressTime = millis();
          digitalWrite(rows[a], LOW);
          return keyboardChars[a][b];
        }
      }
    }
    digitalWrite(rows[a], LOW);
  }
  return '\0';
}