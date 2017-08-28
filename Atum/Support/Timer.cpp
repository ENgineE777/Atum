#include "Timer.h"
#include "Services\Controls\Controls.h"

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
	double cur = GetTickCount() * 0.001;

	if (lastTime < -0.5)
	{
		deltaTime = 0.0f;
	}
	else
	{
		double delta = cur - lastTime;
		deltaTime = (float)delta;
	}

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

	lastTime = cur;

	return deltaTime;
}

int Timer::GetFPS()
{
	return lastFps;
}

const char* Timer::GetTimeStamp()
{
	return stampStr;
}