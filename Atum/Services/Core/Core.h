
#pragma once

#include "Services/Controls/Controls.h"
#include "Services/Font/Fonts.h"
#include "Services/File/Files.h"
#include "Services/Physic/Physics.h"
#include "Services/Render/Render.h"
#include "Services/Scene/SceneManager.h"
#include "Services/Script/Scripts.h"
#include "Services/Sound/Sounds.h"
#include "Services/TaskExecutor/TaskExecutor.h"

#ifdef PLATFORM_ANDROID
extern void CallJavaMethod(const char* function, const char* format, ...);
#endif

/**
\ingroup gr_code_services_core
*/

/**
\brief Core

This is a main object which allows to access to every subsytem of the engine.
Engine supports multi logs, i.e logs from differentent entities can be targeted to a seprated
log file. Logs on Windows stored in subfolder "From dd_mm_yyyy hh_mm" in directore Logs. 

*/

class Core
{
#ifndef DOXYGEN_SKIP

	char  logs_dir[1024];
	float dt = 0.0f;

#endif

public:

	/**
	\brief Access to controls service
	*/
	Controls controls;

	/**
	\brief Access to files service
	*/
	Files files;

	/**
	\brief Access to fonts service
	*/
	Fonts fonts;

	/**
	\brief Access to controls service
	*/
	Physics physics;

	/**
	\brief Access to render service
	*/
	Render render;

	/**
	\brief Access to scene service
	*/
	SceneManager scene_manager;

	/**
	\brief Access to script service
	*/
	Scripts scripts;

	/**
	\brief Access to sound service
	*/
	Sounds sounds;

	/**
	\brief Access to task executor service
	*/
	TaskExecutor taskExecutor;

	/**
	\brief Should be childs clipped by size of a widget

	\param[in] name a name of log file
	\param[in] text formated string of output message

	*/
	void Log(const char* name, const char* text, ...);

#ifndef DOXYGEN_SKIP
	Core();
	virtual ~Core();
	void Init(void* data, void* render_data);

	void Update();
	void CountDeltaTime();
	float GetDeltaTime();
	int GetFPS();
#endif
};

extern Core core;
