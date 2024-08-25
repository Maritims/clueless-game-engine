#ifndef CLUELESS_ENGINE_H
#define CLUELESS_ENGINE_H

#include "CluelessConsole.h"
#include "CluelessKeyboard.h"
#include "CluelessScreen.h"

typedef struct Engine Engine;

typedef void (*EngineFunctionPtr)(Engine*);

struct Engine {
	Screen screen;
	Keyboard keyboard;
	EngineFunctionPtr onUpdate;
	bool isRunning;
};

Engine* engine_construct(wchar_t* windowTitle, int screenWidth, int screenHeight, int fontWidth, int fontHeight, EngineFunctionPtr onCreate, EngineFunctionPtr onUpdate);

void engine_start(Engine* engine);

#endif
