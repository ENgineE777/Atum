#include <windows.h>
#include "Editor/Editor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Editor ed;

	ed.Init();

	return ed.Run();
}