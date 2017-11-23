
#pragma once

#include <cinttypes>

#include <string>
#include <vector>
#include <map>

#ifdef PLATFORM_PC

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <xinput.h>

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
	XINPUT_STATE            joy_prev_states[XUSER_MAX_COUNT];
	XINPUT_STATE            joy_states[XUSER_MAX_COUNT];
	bool                    joy_active[XUSER_MAX_COUNT];

	inline float GetJoyTrigerValue(float val)
	{
		return val / 255.0f;
	}

	inline float GetJoyStickValue(float val)
	{
		val = fmaxf(-1, (float)val / 32767);
		float deadzone = 0.05f;
		val = (abs(val) < deadzone ? 0 : (abs(val) - deadzone) * (val / abs(val)));

		return val /= 1.0f - deadzone;
	}

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
		int         device_index = -1;
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
	std::map<std::string, int> debeugMap;

	std::vector<Alias> aliases;
	std::map<std::string, int> aliasesMap;

	void  ResolveAliases();
	void  CheckDeadEnds(Alias& alias);
	bool  GetHardwareAliasState(int alias, AliasAction action, int device_index);
	float GetHardwareAliasValue(int alias, bool delta, int device_index);

public:

	struct AliasMappig
	{
		std::string name;
		int    alias = -1;

		struct BindName
		{
			int device_index = -1;
			std::string name;
		};

		std::vector<std::vector<BindName>> bindedNames;

		AliasMappig(const char* name);
		bool IsContainHAlias(const char* halias);
	};

	bool  Init(const char* haliases, bool allowDebugKeys);
	bool  LoadAliases(const char* aliases);

	void  SetWindow(void* wnd);

	int   GetAlias(const char* name);

	bool  GetAliasState(int alias, AliasAction action = Activated);
	float GetAliasValue(int alias, bool delta);

	const char* GetActivatedKey(int& device_index);

	bool  DebugKeyPressed(const char* name, AliasAction action = Activated);
	bool  DebugHotKeyPressed(const char* name, const char* name2, const char* name3 = nullptr);

	void  OverrideMousePos(int mx, int my);

	void  Update(float dt);
};

extern Controls controls;