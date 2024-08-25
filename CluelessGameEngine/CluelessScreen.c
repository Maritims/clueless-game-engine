#include "CluelessScreen.h"

bool screen_init(int width, int height, Screen* screen) {
	screen->width = width;
	screen->height = height;
	screen->buffer = calloc((size_t)width * (size_t)height, sizeof(ScreenCharacter));
	if (screen->buffer == NULL) {
		fprintf(stderr, "failed to allocate memory for screen buffer\n");
		return false;
	}

	return true;
}

int screen_draw(Screen* screen, wchar_t wc, Color fgColor, Color bgColor, int x, int y) {
	if (x >= 0 && x < screen->width && y >= 0 && y < screen->height) {
		int i = y * screen->width + x;

		screen->buffer[i].bgColor = bgColor;
		screen->buffer[i].fgColor = fgColor;
		screen->buffer[i].wc = wc;

		return 1;
	}

	return 0;
}

int screen_draw_line(Screen* screen, wchar_t wc, Color fgColor, Color bgColor, int x1, int y1, int x2, int y2) {
	size_t number_of_points = 0;
	bresenham_point_t* points = bresenham_plot(x1, y1, x2, y2, &number_of_points);

	for (size_t i = 0; i < number_of_points; i++) {
		bresenham_point_t point = points[i];
		if (!screen_draw(screen, wc, fgColor, bgColor, point.x, point.y)) {
			fprintf(stderr, "Failed to draw at (%d,%d)\n", point.x, point.y);
			return 0;
		}
	}

	free(points);

	return 1;
}

int screen_draw_box(Screen* screen, wchar_t wc, Color fgColor, Color bgColor, Point topLeft, Point topRight, Point bottomLeft, Point bottomRight) {
	// Top horizontal line.
	screen_draw_line(screen, wc, fgColor, bgColor, topLeft.x, topLeft.y, topRight.x, topRight.y);
	// Left vertical line.
	screen_draw_line(screen, wc, fgColor, bgColor, topLeft.x, topLeft.y, bottomLeft.x, bottomLeft.y);
	// Right vertical line.
	screen_draw_line(screen, wc, fgColor, bgColor, topRight.x, topRight.y, bottomRight.x, bottomRight.y);
	// Bottom horizontal line.
	screen_draw_line(screen, wc, fgColor, bgColor, bottomLeft.x, bottomLeft.y, bottomRight.x, bottomRight.y);

	return 1;
}

int screen_draw_frame(Screen* screen, wchar_t wc, Color fgColor, Color bgColor) {
	screen_draw_box(screen, wc, fgColor, bgColor, (Point) { 0, 0 }, (Point) { screen->width - 1, 0 }, (Point) { 0, screen->height - 1 }, (Point) { screen->width - 1, screen->height - 1 });
}

int screen_draw_string(Screen* screen, const wchar_t* wstr, size_t len, Color fgColor, Color bgColor, int x0, int y0) {
	for (size_t i = 0; i < len; i++) {
		screen_draw(screen, wstr[i], fgColor, bgColor, x0 + i, y0);
	}
	return 1;
}

void screen_fill(Screen* screen, Color color) {
	for (int x = 0; x < screen->width; x++) {
		for (int y = 0; y < screen->height; y++) {
			screen_draw(screen, 0x2588, color, color, x, y);
		}
	}
}

void screen_fill_box(Screen* screen, Color fgColor, Color bgColor, Point topLeft, Point bottomRight) {
	for (int x = topLeft.x; x < bottomRight.x; x++) {
		for (int y = topLeft.y; y < bottomRight.y; y++) {
			screen_draw(screen, PIXEL_TYPE_SOLID, fgColor, bgColor, x, y);
		}
	}
}