#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 64

#define UNLOCK_PIN 26
#define LOCK_PIN 25

extern bool unlocked;
extern unsigned long unlockDuration;

void lockedState();
void unlockedState();
void signalizeWrongPassword();
void signalizePasswordChange();
void handleKey(char key);
String readEEPROMString(int start, int len);
void writeEEPROMString(int start, String value);
void changePassword(String newPassword);