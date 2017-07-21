#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

struct Timer
{
	static __int64 g_nTicksPerSecond;
	static double g_fSecondsPerTick;
	static double g_fMilliSecondsPerTick;
	static unsigned int g_nCPUHerz;

	static void Init() // called once
	{
		QueryPerformanceFrequency ((LARGE_INTEGER*)&g_nTicksPerSecond);

		g_fSecondsPerTick = 1.0 / (double)g_nTicksPerSecond;
		g_fMilliSecondsPerTick = 1000.0 / (double)g_nTicksPerSecond;
	}
	//////////////////////////////////////////////////////////////////////////
	static int GetMillis()
	{
		__int64 ticks;
		GetTicks( &ticks );
		return (int)(TicksToMilliseconds(ticks));
	}
	//////////////////////////////////////////////////////////////////////////
	static double GetMillisFloat()
	{
		__int64 ticks;
		GetTicks( &ticks );
		return g_fMilliSecondsPerTick*ticks;
	}
	//////////////////////////////////////////////////////////////////////////
	static void GetTicks(__int64* pnTime)
	{
		LARGE_INTEGER nTick;
		QueryPerformanceCounter((LARGE_INTEGER*)&nTick);
		*pnTime = nTick.QuadPart;
	}
	//////////////////////////////////////////////////////////////////////////
	static float TicksToSeconds (__int64 nTime)
	{
		return float(g_fSecondsPerTick * nTime);
	}

	//////////////////////////////////////////////////////////////////////////
	static float TicksToMilliseconds (__int64 nTime)
	{
		return float(g_fMilliSecondsPerTick * nTime);
	}
};
