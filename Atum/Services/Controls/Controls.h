
#pragma once

#include <cinttypes>

#include <string>
#include <vector>
#include <map>

#ifdef PLATFORM_PC

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#endif

class Controls
{
public:

	enum AliasAction
	{
		Active,
		Activated
	};

private:

	enum Device
	{
		Keyboard,
		Mouse,
		Joystick
	};

#ifdef PLATFORM_PC
	uint8_t					btns[256];

	LPDIRECTINPUT8			pDI;
	LPDIRECTINPUTDEVICE8	pKeyboard;
	LPDIRECTINPUTDEVICE8	pMouse;

	uint32_t				dwElements;
	byte					diks[256];
	DIMOUSESTATE2			dims2;
	uint32_t				dwMsElements;
	byte					ms_bts[10];
	int						ms_x, ms_y;
	int						prev_ms_x, prev_ms_y;
	HWND					hwnd;
#endif

	struct HardwareAlias
	{
		std::string name;
		Device device;
		int index;
		float value;
	};

	struct AliasRefState
	{
		std::string name;
		int         aliasIndex = -1;
		bool        refer2hardware = false;
	};

	struct AliasRef
	{
		float       modifier = 1.0f;
		std::vector<AliasRefState> refs;
	};

	struct Alias
	{
		std::string name;
		bool visited = false;
		std::vector<AliasRef> aliasesRef;
	};

	bool allowDebugKeys = false;
	std::vector<HardwareAlias> haliases;
	std::vector<Alias> aliases;
	std::map<std::string, int> debeugMap;

	void  ResolveAliases();
	void  CheckDeadEnds(Alias& alias);
	bool  GetHardwareAliasState(int alias, AliasAction action);
	float GetHardwareAliasValue(int alias, bool delta);

public:

	bool  Init(const char* haliases, bool allowDebugKeys);
	bool  LoadAliases(const char* aliases);

	void  SetWindow(void* wnd);

	int   GetAlias(const char* name);
	bool  GetAliasState(int alias, AliasAction action = Activated);
	float GetAliasValue(int alias, bool delta);

	bool  DebugKeyPressed(const char* name, AliasAction action = Activated);
	bool  DebugHotKeyPressed(const char* name, const char* name2, const char* name3 = nullptr);

	void  OverrideMousePos(int mx, int my);

	void  Update(float dt);
};

extern Controls controls;