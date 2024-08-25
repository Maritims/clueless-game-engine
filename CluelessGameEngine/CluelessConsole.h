#ifndef CLUELESS_CONSOLE_H
#define CLUELESS_CONSOLE_H

#include <stdbool.h>
#include "CluelessScreen.h"
#include "CluelessKeyboard.h"

int console_error(wchar_t* format, ...);

int console_init(wchar_t* windowTitle, int screenWidth, int screenHeight, int fontWidth, int fontHeight);

bool console_render(Screen* screen);

void console_read(Keyboard* keyboard);

#endif