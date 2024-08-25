/*
* CluelessWin32Console.c: A Win32 implementation of the CluelessConsole library.
*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#include "CluelessConsole.h"
#include "CluelessKeyboard.h"
#include "CluelessScreen.h"

static wchar_t* s_windowTitle;
static int s_screenWidth;
static int s_screenHeight;
static int s_fontWidth;
static int s_fontHeight;
static double s_fps;
static CONSOLE_SCREEN_BUFFER_INFO s_originalCsbi;
static HANDLE s_consoleInput;
static HANDLE s_consoleOutput;
static HANDLE s_originalConsoleOutput;
static SMALL_RECT s_consoleWindow;

int console_error(wchar_t* format, ...) {
	SetConsoleActiveScreenBuffer(s_originalConsoleOutput);

	DWORD errorCode = GetLastError();
	if (errorCode) {
		wchar_t errorCodeMessage[256];
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			errorCodeMessage,
			256,
			NULL
		);

		va_list args;
		wchar_t final_format[256];
		snprintf(final_format, sizeof(final_format), "ERROR: %ls (%ls) (%d)", format, errorCodeMessage, errorCode);
		va_start(args, format);
		vfprintf(stderr, final_format, args);
		va_end(args);
		return errorCode;
	}
	else {
		va_list args;
		va_start(args, format);
		vwprintf(format, args);
		va_end(args);
		return 0;
	}
}

int console_init(wchar_t* windowTitle, int screenWidth, int screenHeight, int fontWidth, int fontHeight) {
	s_windowTitle = windowTitle;
	s_screenWidth = screenWidth;
	s_screenHeight = screenHeight;
	s_fontWidth = fontWidth;
	s_fontHeight = fontHeight;
	s_consoleInput = GetStdHandle(STD_INPUT_HANDLE);
	s_originalConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	s_consoleOutput = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		NULL,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);

	if (!GetConsoleScreenBufferInfo(s_consoleOutput, &s_originalCsbi)) {
		return console_error(L"GetConsoleScreenBufferInfo");
	}

	// Set console window size to minimum size to allow the screen buffer to shrink.
	// Setting the screen buffer size will fail if it's less than the console window size.
	s_consoleWindow = (SMALL_RECT){ 0, 0, 1, 1 };
	if (!SetConsoleWindowInfo(s_consoleOutput, TRUE, &s_consoleWindow)) {
		return console_error(L"SetConsoleWindowInfo");
	}

	// Set the size of the screen buffer now that it cannot possibly be smaller than the window size. This should work!
	if (!SetConsoleScreenBufferSize(s_consoleOutput, (COORD) { s_screenWidth, s_screenHeight })) {
		return console_error(L"SetConsoleScreenBufferSize");
	}

	// Assign the screen buffer to the console.
	if (!SetConsoleActiveScreenBuffer(s_consoleOutput)) {
		return console_error(L"SetConsoleActiveScreenBuffer");
	}

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = s_fontWidth;
	cfi.dwFontSize.Y = s_fontHeight;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, LF_FACESIZE, L"Consolas");

	if (!SetCurrentConsoleFontEx(s_consoleOutput, FALSE, &cfi)) {
		return console_error(L"SetCurrentConsoleFontEx");
	}

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(s_consoleOutput, &csbi)) {
		return console_error(L"GetConsoleScreenBufferInfo");
	}

	if (s_screenWidth > csbi.dwMaximumWindowSize.X) {
		return console_error(L"Desired width (%d) exceeds maximum window width (%d)", s_screenWidth, csbi.dwMaximumWindowSize.X);
	}

	if (s_screenHeight > csbi.dwMaximumWindowSize.Y) {
		return console_error(L"Desired height (%d) exceeds maximum window height (%d)", s_screenHeight, csbi.dwMaximumWindowSize.Y);
	}

	// Set actual window size now that we know that we're within the boundaries.
	s_consoleWindow = (SMALL_RECT){ 0, 0, s_screenWidth - 1, s_screenHeight - 1 };
	if (!SetConsoleWindowInfo(s_consoleOutput, TRUE, &s_consoleWindow)) {
		return console_error(L"SetConsoleWindowInfo");
	}

	return 1;
}

bool console_render(Screen* screen) {
	DWORD buffer_size = s_screenWidth * s_screenHeight;
	CHAR_INFO* buffer = calloc(buffer_size, sizeof(CHAR_INFO));
	if (buffer == NULL) {
		return false;
	}

	for (int x = 0; x < s_screenWidth; x++) {
		for (int y = 0; y < s_screenHeight; y++) {
			int i = y * s_screenWidth + x;
			buffer[i].Char.UnicodeChar = screen->buffer[i].wc;
			buffer[i].Attributes = screen->buffer[i].fgColor | screen->buffer[i].bgColor;
		}
	}

	WriteConsoleOutput(s_consoleOutput, buffer, (COORD) { s_screenWidth, s_screenHeight }, (COORD) { 0, 0 }, & s_consoleWindow);
	
	wchar_t actual_title[128];
	swprintf(actual_title, 128, L"%ls - %dx%d - FPS: %.2f", s_windowTitle, s_screenWidth, s_screenHeight, s_fps);
	SetConsoleTitle(actual_title);

	free(buffer);

	return true;
}

void console_read(Keyboard* keyboard) {
	for (int i = 0; i < 256; i++) {
		keyboard->newState[i] = GetAsyncKeyState(i);
		keyboard->keys[i].isHeld = false;

		if (keyboard->newState[i] != keyboard->oldState[i]) {
			if (keyboard->newState[i] & 0x8000) {
				keyboard->keys[i].isHeld = true;
			}
		}

		keyboard->oldState[i] = keyboard->newState[i];
	}
}