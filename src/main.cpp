#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 64

// ROWS and COLS configuration
const byte ROWS = 4;
const byte COLS = 3;

const byte PASSWORD_LENGTH = 4;

const byte UNLOCK_PIN = 26;
const byte LOCK_PIN = 25;

byte rows[ROWS] = {5, 13, 12, 14};  // Row pins
byte columns[COLS] = {19, 18, 23};  // Column pins

bool unlocked = false;
long unlockedAt = 0;

char znaky[ROWS][COLS] = {
    {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};

const int unlockDuration = 5000;
int debounceDelay = 300;
long lastKeyPressTime = 0;

String accessPassword = "1234";

bool enteringSpecialCode = false;

String currentInput = "";

char get_keyboard_key();
void lockedState();
void unlockedState();
void signalizeWrongPassword();
void signalizePasswordChange();
void handleKey(char key);
String readEEPROMString(int start, int len);
void writeEEPROMString(int start, String value);

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < ROWS; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], LOW);
  }

  for (int i = 0; i < COLS; i++) {
    pinMode(columns[i], INPUT_PULLDOWN);
  }

  pinMode(UNLOCK_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);

  digitalWrite(LOCK_PIN, HIGH);
  digitalWrite(UNLOCK_PIN, LOW);

  EEPROM.begin(EEPROM_SIZE);
  accessPassword = readEEPROMString(0, 16);

  Serial.println(String("Password from EEPROM: ") + accessPassword);

  if (accessPassword == "") {
    accessPassword = "1234";
    writeEEPROMString(0, accessPassword);
  }

  Serial.println(String("Password: ") + accessPassword);
}

void loop() {
  if (unlocked && millis() - unlockedAt >= unlockDuration) {
    lockedState();
  }

  char key_pressed = get_keyboard_key();
  if (key_pressed != '\0') {
    Serial.println(String("Key pressed: ") + key_pressed);
    handleKey(key_pressed);
  }
}

void lockedState() {
  unlocked = false;
  digitalWrite(LOCK_PIN, HIGH);
  digitalWrite(UNLOCK_PIN, LOW);
  Serial.println("Locked");
}

void unlockedState() {
  unlocked = true;
  unlockedAt = millis();
  digitalWrite(LOCK_PIN, LOW);
  digitalWrite(UNLOCK_PIN, HIGH);
  Serial.println("Unlocked");
}

void handleKey(char key) {
  currentInput += key;

  if (key == '#' && enteringSpecialCode == false) {
    // format: #1234*5678
    enteringSpecialCode = true;
    Serial.println(String("Entering special code..."));
    return;
  }

  if (enteringSpecialCode) {
    if (currentInput.length() == 6) {
      if (key != '*') {
        signalizeWrongPassword();
        currentInput = "";
        enteringSpecialCode = false;
      }
    } else if (currentInput.length() == 10) {
      String previousPassword = currentInput.substring(1, 5);
      String newPassword = currentInput.substring(6, 10);
      if (accessPassword != previousPassword) {
        signalizeWrongPassword();
      } else {
        accessPassword = newPassword;
        writeEEPROMString(0, accessPassword);
        signalizePasswordChange();
      }
      currentInput = "";
      enteringSpecialCode = false;
    }
    return;
  }

  if (currentInput.length() == PASSWORD_LENGTH) {
    if (currentInput == accessPassword) {
      unlockedState();
      currentInput = "";
    } else {
      Serial.println(String("Invalid password"));
      currentInput = "";
      signalizeWrongPassword();
    }
  }
}

String readEEPROMString(int start, int len) {
  String result = "";
  for (int i = start; i < start + len; i++) {
    char c = EEPROM.read(i);
    if (c == 0 || c == 255) break;
    result += c;
  }
  return result;
}

void writeEEPROMString(int start, String value) {
  for (int i = 0; i < value.length(); i++) {
    EEPROM.write(start + i, value[i]);
  }
  EEPROM.write(start + value.length(), 0);
  EEPROM.commit();
}

char get_keyboard_key() {
  for (int a = 0; a < ROWS; a++) {
    // Set the current row HIGH
    digitalWrite(rows[a], HIGH);

    for (int b = 0; b < COLS; b++) {
      if (digitalRead(columns[b]) == HIGH) {
        if (millis() - lastKeyPressTime > debounceDelay) {
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

void signalizeWrongPassword() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LOCK_PIN, LOW);
    delay(200);
    digitalWrite(LOCK_PIN, HIGH);
    delay(200);
  }
}

void signalizePasswordChange() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(UNLOCK_PIN, HIGH);
    delay(200);
    digitalWrite(UNLOCK_PIN, LOW);
    delay(200);
  }
}