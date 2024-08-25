#include <string.h>
#include "CluelessKeyboard.h"

void keyboard_init(Keyboard* keyboard) {
	memset(keyboard->oldState, 0, sizeof(keyboard->oldState));
	memset(keyboard->newState, 0, sizeof(keyboard->newState));
	for (int i = 0; i < 256; i++) {
		keyboard->keys[i] = (KeyboardKey){
			.isHeld = false
		};
	}
}