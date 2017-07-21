
#pragma once

#include "Support/Delegate.h"
#include <cinttypes>

#ifdef PLATFORM_PC

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#endif

class Controls : public Object
{	
public:

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

	enum AliasAction
	{
		Active,
		Activated
	};

	bool  Init(void* data);

	int   GetAlias(const char* name);
	bool  GetAliasState(int alias, bool exclusive, AliasAction action);
	float GetAliasValue(int alias, bool delta);
	void  SetAliasValue(int alias, float value);

	void  Update(float dt);
};

extern Controls controls;