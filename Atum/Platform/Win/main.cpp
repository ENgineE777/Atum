#include <windows.h>
#include "Editor/Editor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (strstr(lpCmdLine, "-gen_script_help"))
	{
		CreateDirectory("script_help", nullptr);

		core.scripts.help_out_dir = true;
		core.scripts.Start();
		core.scripts.Stop();

		return 0;
	}

	editor.Init();

	return editor.Run();
}