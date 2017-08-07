#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct Timer
{
	static float  lastTime;
	static float  deltaTime;
	static int    lastFps;
	static int    curFps;
	static float  fpsTimer;
	static double totalTime;

	static int   stampMin;
	static float stampSec;
	static char  stampStr[32];

	static float CountDeltaTime();
	static int  GetFPS();
	static const char* GetTimeStamp();
};
