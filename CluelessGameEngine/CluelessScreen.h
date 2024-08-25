#ifndef CLUELESS_SCREEN_H
#define CLUELESS_SCREEN_H

/*
* CluelessScreen.h: A representation of a screen in the Clueless Game Engine.
* This library is and shall remain without platform specific code.
*/

#include <stdbool.h>
#include "Bresenham.h"

typedef enum Color {
	FG_BLACK = 0x0000,
	FG_BLUE = 0x0001,
	FG_GREEN = 0x0002,
	FG_AQUA = 0x0003,
	FG_RED = 0x0004,
	FG_PURPLE = 0x0005,
	FG_YELLOW = 0x0006,
	FG_WHITE = 0x0007,
	FG_GRAY = 0x0008,
	FG_LIGHT_BLUE = 0x0009,
	FG_LIGHT_GREEN = 0x000A,
	FG_LIGHT_AQUA = 0x000B,
	FG_LIGHT_RED = 0x000C,
	FG_LIGHT_PURPLE = 0x000D,
	FG_LIGHT_YELLOW = 0x000E,
	FG_BRIGHT_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_BLUE = 0x0010,
	BG_GREEN = 0x0020,
	BG_AQUA = 0x0030,
	BG_RED = 0x0040,
	BG_PURPLE = 0x0050,
	BG_YELLOW = 0x0060,
	BG_WHITE = 0x0070,
	BG_GRAY = 0x0080,
	BG_LIGHT_BLUE = 0x0090,
	BG_LIGHT_GREEN = 0x00A0,
	BG_LIGHT_AQUA = 0x00B0,
	BG_LIGHT_RED = 0x00C0,
	BG_LIGHT_PURPLE = 0x00D0,
	BG_LIGHT_YELLOW = 0x00E0,
	BG_BRIGHT_WHITE = 0x00F0
} Color;

typedef enum PixelType {
	PIXEL_TYPE_SOLID = 0x2588
} PixelType;

typedef struct {
	wchar_t wc;
	Color fgColor;
	Color bgColor;
} ScreenCharacter;

typedef struct {
	int width;
	int height;
	ScreenCharacter* buffer;
	double fps;
} Screen;

typedef struct {
	int x;
	int y;
} Point;

bool screen_init(int width, int height, Screen* screen);

int screen_draw(Screen* screen, wchar_t wc, Color fgColor, Color bgColor, int x, int y);

int screen_draw_line(Screen* screen, wchar_t wc, Color fgColor, Color bgColor, int x1, int y1, int x2, int y2);

int screen_draw_string(Screen* screen, const wchar_t* wstr, size_t len, Color fgColor, Color bgColor, int x0, int y0);

int screen_draw_box(Screen* screen, wchar_t wc, Color fgColor, Color bgColor, Point topLeft, Point topRight, Point bottomLeft, Point bottomRight);

int screen_draw_frame(Screen* screen, wchar_t wc, Color fgColor, Color bgColor);

void screen_fill(Screen* screen, Color color);

void screen_fill_box(Screen* screen, Color fgColor, Color bgColor, Point topLeft, Point bottomRight);

#endif