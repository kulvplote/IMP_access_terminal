#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Arduino.h>

// ROWS and COLS configuration
const byte ROWS = 4;
const byte COLS = 3;

extern byte rows[ROWS];
extern byte columns[COLS];
extern char keyboardChars[ROWS][COLS];
extern int debounceDelay;
extern long lastKeyPressTime;

char get_keyboard_key();

#endif