#include <windows.h>
#include "CluelessClock.h"

double clock_now() {
	LARGE_INTEGER frequency, counter;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);

	return (double)counter.QuadPart * 1000.0 * 1000.0 * 1000.0 / frequency.QuadPart;
}