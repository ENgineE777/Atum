
#pragma once

#include "Services/Controls/Controls.h"
#include "Services/Font/Fonts.h"
#include "Services/Physic/Physics.h"
#include "Services/Render/Render.h"
#include "Services/TaskExecutor/TaskExecutor.h"

#ifdef PLATFORM_ANDROID
extern void CallJavaMethod(const char* function, const char* format, ...);
#endif

class Core
{
	char  logs_dir[1024];
	float dt = 0.0f;

public:

	Core();
	virtual ~Core();

	void Init(void* data);
	void Log(const char* name, const char* text, ...);
	void Update();
	void CountDeltaTime();
	float GetDeltaTime();
	int GetFPS();
};

extern Core core;
