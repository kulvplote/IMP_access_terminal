#include <Arduino.h>

// ROWS and COLS configuration
const byte ROWS = 4;
const byte COLS = 3;

const byte UNLOCK_PIN = 26;
const byte LOCK_PIN = 25;

byte rows[ROWS] = {5, 13, 12, 14};  // Row pins
byte columns[COLS] = {19, 18, 23};  // Column pins

bool locked = true;

char znaky[ROWS][COLS] = {
    {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};

int debounceDelay = 200;
long lastKeyPressTime = 0;

void setup() {
  for (int i = 0; i < ROWS; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], LOW);  // Set initial state to LOW
  }

  for (int i = 0; i < COLS; i++) {
    pinMode(columns[i], INPUT_PULLDOWN);
  }

  pinMode(UNLOCK_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);

  digitalWrite(LOCK_PIN, HIGH);
  digitalWrite(UNLOCK_PIN, LOW);

  Serial.begin(115200);
}

char get_keyboard_key;

void loop() { get_keyboard_key(); }

char get_keyboard_key() {
  for (int a = 0; a < ROWS; a++) {
    // Set the current row HIGH
    digitalWrite(rows[a], HIGH);

    for (int b = 0; b < COLS; b++) {
      if (digitalRead(columns[b]) == HIGH) {
        if (millis() - lastKeyPressTime > debounceDelay) {
          Serial.println(znaky[a][b]);
          lastKeyPressTime = millis();
          digitalWrite(rows[a], LOW);
          return znaky[a][b];
        }
      }
    }
    digitalWrite(rows[a], LOW);
  }
  return '\0';
}
