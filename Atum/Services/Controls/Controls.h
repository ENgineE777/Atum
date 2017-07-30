
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

	LPDIRECTINPUT8          pDI;
	LPDIRECTINPUTDEVICE8    pKeyboard;
	LPDIRECTINPUTDEVICE8    pMouse;

	uint32_t				dwElements;
	byte					diks[256];
	DIMOUSESTATE2			dims2;
	uint32_t				dwMsElements;
	byte					ms_bts[10];
	int						ms_x, ms_y;
	int						prev_ms_x, prev_ms_y;
#endif

	struct HardwareAlias
	{
		std::string name;
		Device device;
		int index;
		float value;
	};

	struct AliasRef
	{
		std::string name;
		float       modifier;
		int         aliasIndex;
		bool        refer2hardware;
		AliasRef()
		{
			modifier = 1.0f;
			aliasIndex = -1;
			refer2hardware = false;
		};
	};

	struct Alias
	{
		std::string name;
		bool visited;
		std::vector<AliasRef> aliasesRef;
		Alias()
		{
			visited = false;
		}
	};

	std::vector<HardwareAlias> haliases;
	std::vector<Alias> aliases;
	std::map<std::string, int> debeugMap;

	void ResolveAliases();
	void CheckDeadEnds(Alias& alias);
	bool  GetHardwareAliasState(int alias, bool exclusive, AliasAction action);
	float GetHardwareAliasValue(int alias, bool delta);

public:

	bool  Init(void* data, const char* haliases, const char* aliases);

	int   GetAlias(const char* name);
	bool  GetAliasState(int alias, bool exclusive, AliasAction action);
	float GetAliasValue(int alias, bool delta);

	bool  DebugKeyPressed(const char* name, AliasAction action = Activated);

	void  OverrideMousePos(int mx, int my);

	void  Update(float dt);
};

extern Controls controls;