#include <windows.h>
#include <windowsx.h>

#include "Support/Timer.h"
#include "Services/Render/Render.h"
#include "Services/Controls/Controls.h"
#include "SceneObjects/programs.h"
#include "SceneObjects/scene2.h"
#include <time.h>

HWND hwnd;
Scene2 scene;

const char g_szClassName[] = "LectureClass";

void InitRender(int width, int height)
{
	srand(time(NULL));

	Timer::Init();

	render.Init("DX11", width, height, &hwnd);

	//render.AddExecutedDelgateList("prepare", 0);
	//render.AddExecutedDelgateList("shadows", 0);
	//render.AddExecutedDelgateList("camera", 0);
	//render.AddExecutedDelgateList("main", 0);

	//render.AddExecutedDelgateList("controls", 0);

	controls.Init(&hwnd, "settings/controls/hardware_pc", "settings/controls/user_pc");

	Programs::Init();
	scene.Init();
}

void Draw(float dt)
{
	render.Execute(dt);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		case WM_MOUSEMOVE:
		{
			controls.OverrideMousePos(GET_Y_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG Msg;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Lecture: Render System",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	InitRender(800, 600);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	float prev_t = Timer::GetMillisFloat();

	MSG msg;
	bool done = false;

	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			float t = Timer::GetMillisFloat();
			float dt = (t - prev_t) * 0.001f;
			prev_t = t;

			Draw(dt);
		}
	}

	return 0;
}