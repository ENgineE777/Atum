#pragma once

/**
\ingroup gr_code_common
*/

/**
\brief System timer

This class needed for proper calculation delta time beetween frames 

*/

struct Timer
{
#ifndef DOXYGEN_SKIP
	static double lastTime;
	static float  deltaTime;
	static int    lastFps;
	static int    curFps;
	static float  fpsTimer;
	static double totalTime;

	static int   stampMin;
	static float stampSec;
	static char  stampStr[32];
#endif

	/**
	\brief Update internal values. This update needed to be called every tick for proper calculation
	*/

	static float CountDeltaTime();

	/**
	\brief Get curent Delta Time
	*/
	static float GetDeltaTime();

	/**
	\brief Get curent FPS
	*/
	static int   GetFPS();

	/**
	\brief Get curent timestamp in string form	*/
	static const char* GetTimeStamp();
};
