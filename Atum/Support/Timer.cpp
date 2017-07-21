#include "Timer.h"

__int64 Timer::g_nTicksPerSecond = 1000000000;
double Timer::g_fSecondsPerTick = 1e-9;
double Timer::g_fMilliSecondsPerTick = 1e-6;
unsigned int Timer::g_nCPUHerz = 1000000000;