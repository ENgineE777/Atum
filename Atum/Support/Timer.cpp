#include "Timer.h"
#include "Services\Controls\Controls.h"

#ifdef PLATFORM_PC
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef PLATFORM_ANDROID
struct timespec lastTimeSpec;
#endif

double Timer::lastTime = -1.0;
float  Timer::deltaTime = 0.0f;
int    Timer::lastFps = 0;
int    Timer::curFps = 0;
float  Timer::fpsTimer = 0.0f;
double Timer::totalTime = 0.0;

int   Timer::stampMin = 0;
float Timer::stampSec = 0;
char  Timer::stampStr[32] = "00:00 - ";

float Timer::CountDeltaTime()
{
#ifdef PLATFORM_ANDROID
	struct timespec currTime;
	clock_gettime(CLOCK_MONOTONIC, &currTime);
#endif

#ifdef PLATFORM_PC
	double cur = GetTickCount() * 0.001;
#endif

	if (lastTime < -0.5)
	{
		deltaTime = 0.0f;
	}
	else
	{
#ifdef PLATFORM_PC
		double delta = cur - lastTime;
		deltaTime = (float)delta;
#endif

#ifdef PLATFORM_ANDROID
		long iNanoSeconds = (currTime.tv_sec > lastTimeSpec.tv_sec) ? (1000000000 - lastTimeSpec.tv_nsec) + currTime.tv_nsec : currTime.tv_nsec - lastTimeSpec.tv_nsec;
		deltaTime = (float)iNanoSeconds / 1000000000.0f;
#endif
	}

#ifdef PLATFORM_ANDROID
	lastTimeSpec = currTime;
#endif

	totalTime += deltaTime;
	stampSec += deltaTime;

	if (stampSec>60.0f)
	{
		int num = (int)(stampSec / 60.0f);
		stampMin += num;
		stampSec -= num * 60.0f;
	}

	sprintf(stampStr, "%i:%2.3f - ", stampMin, stampSec);

	if (deltaTime<0.0f) deltaTime = 0.0f;

	if (deltaTime>0.3f) deltaTime = 0.3f;

	curFps++;
	fpsTimer += deltaTime;

	if (fpsTimer>1.0f)
	{
		lastFps = curFps;
		curFps = 0;
		fpsTimer -= 1.0f;
	}

	if (controls.DebugKeyPressed("KEY_X", Controls::Active))
	{
		deltaTime *= 0.4f;
	}
	else
	if (controls.DebugKeyPressed("KEY_Q", Controls::Active))
	{
		deltaTime *= 3.0f;
	}

#ifdef PLATFORM_PC
	lastTime = cur;
#endif

#ifdef PLATFORM_ANDROID
	lastTime = 0;
#endif;

	return deltaTime;
}

int Timer::GetFPS()
{
	return lastFps;
}

float Timer::GetDeltaTime()
{
	return deltaTime;
}

const char* Timer::GetTimeStamp()
{
	return stampStr;
}