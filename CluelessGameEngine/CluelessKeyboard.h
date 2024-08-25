#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

#ifdef _WIN32
#include "CluelessWin32KeyCodes.h"
#endif

#ifdef linux
#include "CluelessLinuxKeyCodes.h"
#endif

typedef struct {
	bool isHeld;
} KeyboardKey;

typedef struct {
	int oldState[256];
	int newState[256];
	KeyboardKey keys[256];
} Keyboard;

void keyboard_init(Keyboard* keyboard);

#endif
