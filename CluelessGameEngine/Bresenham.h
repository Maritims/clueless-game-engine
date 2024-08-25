#ifndef BRESENHAM_H
#define BRESENHAM_H

/*
* Bresenham's line algorithm.
* https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct bresenham_point_t {
	int x;
	int y;
} bresenham_point_t;

bresenham_point_t* bresenham_plot(int x0, int y0, int x1, int y1, size_t* number_of_points);

#endif BRESENHAM_H
