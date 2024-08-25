#include "Bresenham.h"

bresenham_point_t* bresenham_plot(int x0, int y0, int x1, int y1, size_t* number_of_points) {
	int horizontal_distance = abs(x1 - x0);
	int vertical_distance = abs(y1 - y0);
	int horizontal_step = (x0 < x1) ? 1 : -1;
	int vertical_step = (y0 < y1) ? 1 : -1;

	// The error term helps us decide when to step in the y direction as we move along the x axis.
	int error_term = horizontal_distance - vertical_distance;

	// This is always 2*error_term and is used to determine whether to adjust the x coordinate or the y coordinate.
	int e2;

	int capacity = 10;
	bresenham_point_t* points = calloc(capacity, sizeof(bresenham_point_t));
	if (points == NULL) {
		fprintf(stderr, "Failed to allocate memory for Bresenham points\n");
		return 0;
	}

	int i = 0;
	while (1) {
		if (i >= capacity) {
			int new_capacity = capacity * 2;
			bresenham_point_t* new_points = realloc(points, new_capacity * sizeof(bresenham_point_t));
			if (new_points == NULL) {
				fprintf(stderr, "Failed to allocate additional memory for the Bresenham points\n");
				free(points);
				return NULL;
			}

			capacity = new_capacity;
			points = new_points;
		}

		points[i++] = (bresenham_point_t){ x0, y0 };
		(*number_of_points)++;

		if (x0 == x1 && y0 == y1) {
			return points;
		}

		e2 = 2 * error_term;
		if (e2 > -vertical_distance) {
			error_term -= vertical_distance;
			x0 += horizontal_step;
		}

		if (e2 < horizontal_distance) {
			error_term += horizontal_distance;
			y0 += vertical_step;
		}
	}

	free(points);
	return NULL;
}