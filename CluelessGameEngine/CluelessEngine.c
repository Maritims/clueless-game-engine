#include "CluelessClock.h"
#include "CluelessEngine.h"
#include "CluelessExitCodes.h"
#include "CluelessConsole.h"

static void __engine_loop(void* ptr) {
	Engine* engine = (Engine*)ptr;
	double t1 = clock_now();

	while (engine->isRunning) {
		double t2 = clock_now();
		double delta = t2 - t1;
		t1 = t2;
		engine->screen.fps = 1.0 / delta;

		console_read(&engine->keyboard);
		engine->onUpdate(engine);
		console_render(&engine->screen);
	}
}

#ifdef _WIN32
#include <windows.h>

HANDLE s_gameThread = NULL;

static void __engine_create_thread(Engine* engine) {
	DWORD threadId;
	s_gameThread = CreateThread(NULL, 0, __engine_loop, engine, 0, &threadId);
}

static void __engine_await_thread() {
	WaitForSingleObject(s_gameThread, INFINITE);
	CloseHandle(s_gameThread);
}
#endif

Engine* engine_construct(wchar_t* windowTitle, int screenWidth, int screenHeight, int fontWidth, int fontHeight, EngineFunctionPtr onCreate, EngineFunctionPtr onUpdate) {
	if (!console_init(windowTitle, screenWidth, screenHeight, fontWidth, fontHeight)) {
		//fprintf(stderr, "Unable to initialise console\n");
		return NULL;
	}

	Engine* engine = malloc(sizeof(Engine));
	if (engine == NULL) {
		fprintf(stderr, "Failed to allocate memory for engine\n");
		return NULL;
	}

	engine->isRunning = false;
	engine->onUpdate = onUpdate;

	if (!screen_init(screenWidth, screenHeight, &engine->screen)) {
		free(engine);
		return NULL;
	}

	keyboard_init(&engine->keyboard);

	if (onCreate) {
		onCreate(engine);
	}

	return engine;
}

void engine_start(Engine* engine) {
	if (!engine) {
		console_error(L"engine_start");
		return;
	}

	__engine_create_thread(engine);
	engine->isRunning = true;
	__engine_await_thread();
}