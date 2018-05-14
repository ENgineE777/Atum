#pragma once

struct Timer
{
	static double lastTime;
	static float  deltaTime;
	static int    lastFps;
	static int    curFps;
	static float  fpsTimer;
	static double totalTime;

	static int   stampMin;
	static float stampSec;
	static char  stampStr[32];

	static float CountDeltaTime();
	static float GetDeltaTime();
	static int   GetFPS();

	static const char* GetTimeStamp();
};
