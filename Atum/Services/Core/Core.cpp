
#include "Core.h"
#include "Support/Timer.h"
#include <ctime>

#include "Services/Scene/SceneObjectComp.h"

#ifdef PLATFORM_ANDROID
#include <android/log.h>
#endif

#ifdef EDITOR
#include "Editor/Editor.h"
#endif

Core core;

Core::Core()
{
	logs_dir[0] = 0;
}

Core::~Core()
{

}

void Core::Init(void* data)
{
    files.Init();
    
	srand((unsigned int)time(nullptr));
	StringUtils::Init();

	for (const auto& decl : ClassFactorySceneObjectComp::Decls())
	{
		decl->Init();
	}

	Timer::CountDeltaTime();

#ifdef PLATFORM_PC
	char cur_dir[1024];
	GetCurrentDirectory(1024, cur_dir);
	StringUtils::Printf(logs_dir, 1024, "%s/Logs", cur_dir);
	CreateDirectory(logs_dir, nullptr);

	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);

	StringUtils::Printf(logs_dir, 1024, "%s/Logs/From %i_%i_%i %i_%i", cur_dir, now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec);
	CreateDirectory(logs_dir, nullptr);


	controls.Init("settings/controls/hardware_pc", true);
	controls.LoadAliases("settings/controls/user_pc");
	controls.SetWindow(data);
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
	controls.Init("settings/controls/hardware_mobile", true);
	controls.LoadAliases("settings/controls/user_pc");
#endif

	fonts.Init();

	render.Init("DX11", 100, 100, data);

	physics.Init();
}

void Core::Log(const char* name, const char* text, ...)
{
	static char buffer[4096];
	va_list args;
	va_start(args, text);

	vsnprintf(buffer, sizeof(buffer) - 4, text, args);

	va_end(args);

#ifdef PLATFORM_PC
	OutputDebugString(buffer);
	OutputDebugString("\n");

	char path[1024];
	StringUtils::Printf(path, 1024, "%s/%s.txt", logs_dir, name);

	FILE* f = fopen(path, "a");

	if (f)
	{
		fwrite(Timer::GetTimeStamp(), strlen(Timer::GetTimeStamp()), 1, f);
		fwrite(buffer, strlen(buffer), 1, f);
		fwrite("\n", strlen("\n"), 1, f);
		fclose(f);
	}
#endif

#ifdef PLATFORM_ANDROID
	__android_log_print(ANDROID_LOG_ERROR, name, "%s", buffer);
#endif

#ifdef EDITOR
	Editor::LogToOutputBox(name, buffer);
#endif
}

void Core::CountDeltaTime()
{
	Timer::CountDeltaTime();
	dt = Timer::GetDeltaTime();
}

void Core::Update()
{
	physics.Update(dt);

	render.Execute(dt);

	controls.Update(dt);

	physics.Fetch();
}

float Core::GetDeltaTime()
{
	return dt;
}

int Core::GetFPS()
{
	return Timer::GetFPS();
}
