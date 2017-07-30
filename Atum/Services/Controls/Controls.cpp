
#include "Controls.h"
#include "Services/Render/Render.h"
#include "Support/json/JSONReader.h"


Controls controls;

bool Controls::Init(void* data, const char* name_haliases, const char* name_aliases)
{
#ifdef PLATFORM_PC
	HWND hwnd = *((HWND*)data);

	if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&pDI, NULL)))
	{
		return false;
	}

	if (FAILED(pDI->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL)))
	{
		return false;
	}

	if (FAILED(pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	pKeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 16;

	if (FAILED(pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
	{
		return false;
	}

	pKeyboard->Acquire();

	if (FAILED(pDI->CreateDevice(GUID_SysMouse, &pMouse, NULL)))
	{
		return false;
	}

	if (FAILED(pMouse->SetDataFormat(&c_dfDIMouse2)))
	{
		return false;
	}

	pMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 16;

	if (FAILED(pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
	{
		return false;
	}

	pMouse->Acquire();

	ms_x = ms_y = 0;
	prev_ms_x = prev_ms_y = -1000;
#endif

	JSONReader* reader = new JSONReader();

	if (reader->Parse(name_haliases))
	{
		while (reader->EnterBlock("keyboard"))
		{
			haliases.push_back(HardwareAlias());
			HardwareAlias& halias = haliases[haliases.size() - 1];

			halias.device = Keyboard;
			reader->Read("name", halias.name);
			reader->Read("index", halias.index);

			debeugMap[halias.name] = haliases.size() - 1;

			reader->LeaveBlock();
		}

		while (reader->EnterBlock("mouse"))
		{
			haliases.push_back(HardwareAlias());
			HardwareAlias& halias = haliases[haliases.size() - 1];

			halias.device = Mouse;
			reader->Read("name", halias.name);
			reader->Read("index", halias.index);

			debeugMap[halias.name] = haliases.size() - 1;

			reader->LeaveBlock();
		}
	}

	reader->Release();

	reader = new JSONReader();

	if (reader->Parse(name_aliases))
	{
		while (reader->EnterBlock("Aliases"))
		{
			aliases.push_back(Alias());
			Alias& alias = aliases[aliases.size() - 1];

			reader->Read("name", alias.name);

			while (reader->EnterBlock("AliasesRef"))
			{
				alias.aliasesRef.push_back(AliasRef());
				AliasRef& aliasRef = alias.aliasesRef[alias.aliasesRef.size() - 1];

				reader->Read("name", aliasRef.name);
				reader->Read("modifier", aliasRef.modifier);

				reader->LeaveBlock();
			}

			reader->LeaveBlock();
		}
	}

	reader->Release();

	ResolveAliases();

	return true;
}

void Controls::ResolveAliases()
{
	for (int i = 0; i < aliases.size(); i++)
	{
		Alias& alias = aliases[i];

		for (int j = 0; j < alias.aliasesRef.size(); j++)
		{
			AliasRef& aliasRef = alias.aliasesRef[j];

			for (int l = 0; l < aliases.size(); l++)
			{
				if (i == l)
				{
					continue;
				}

				if (StringUtils::IsEqual(aliases[l].name.c_str(), aliasRef.name.c_str()))
				{
					aliasRef.aliasIndex = l;
					aliasRef.refer2hardware = false;
					break;
				}
			}

			if (aliasRef.aliasIndex == -1)
			{
				for (int l = 0; l < haliases.size(); l++)
				{
					if (StringUtils::IsEqual(haliases[l].name.c_str(), aliasRef.name.c_str()))
					{
						aliasRef.aliasIndex = l;
						aliasRef.refer2hardware = true;
						break;
					}
				}
			}
		}
	}

	for (int i = 0; i < aliases.size(); i++)
	{
		CheckDeadEnds(aliases[i]);
	}
}

void Controls::CheckDeadEnds(Alias& alias)
{
	alias.visited = true;

	for (int j = 0; j < alias.aliasesRef.size(); j++)
	{
		AliasRef& aliasRef = alias.aliasesRef[j];

		if (aliasRef.aliasIndex != -1 && !aliasRef.refer2hardware)
		{
			if (aliases[aliasRef.aliasIndex].visited)
			{
				aliasRef.aliasIndex = -1;
			}
			else
			{
				CheckDeadEnds(aliases[aliasRef.aliasIndex]);
			}
		}
	}

	alias.visited = false;
}

int Controls::GetAlias(const char* name)
{
	for (int i = 0; i < aliases.size(); i++)
	{
		if (StringUtils::IsEqual(aliases[i].name.c_str(), name))
		{
			return i;
		}
	}

	return -1;
}

bool Controls::GetHardwareAliasState(int index, bool exclusive, AliasAction action)
{
	HardwareAlias& halias = haliases[index];

	if (halias.device == Keyboard)
	{
		if (action == Activated)
		{
			if (exclusive && btns[halias.index] == 1)
			{
				btns[halias.index]++;
				return true;
			}

			return (btns[halias.index] == 1);
		}

		if (action == Active)
		{
			return (btns[halias.index] > 0);
		}
	}
	else
	if (halias.device == Mouse)
	{
		if (halias.index < 10)
		{
			if (action == Activated)
			{
				if (exclusive && ms_bts[halias.index] == 1)
				{
					ms_bts[halias.index]++;
					return true;
				}

				return (ms_bts[halias.index] == 1);
			}

			if (action == Active)
			{
				return (ms_bts[halias.index] > 0);
			}
		}
	}

	return false;
}

bool Controls::GetAliasState(int index, bool exclusive, AliasAction action)
{
	if (index == -1)
	{
		return 0.0f;
	}

	Alias& alias = aliases[index];

	for (int i = 0; i < alias.aliasesRef.size(); i++)
	{
		AliasRef& aliasRef = alias.aliasesRef[i];

		if (aliasRef.aliasIndex == -1)
		{
			continue;
		}

		float val = 0.0f;

		if (aliasRef.refer2hardware)
		{
			val = GetHardwareAliasState(aliasRef.aliasIndex, exclusive, action);
		}
		else
		{
			val = GetHardwareAliasState(aliasRef.aliasIndex, exclusive, action);
		}

		if (fabs(val) > 0.01f)
		{
			return val * aliasRef.modifier;
		}
	}

	return 0.0f;
}

float Controls::GetHardwareAliasValue(int index, bool delta)
{
	HardwareAlias& halias = haliases[index];

	if (halias.device == Keyboard)
	{
		if (btns[halias.index])
		{
			return 1.0f;
		}
	}

	if (halias.device == Mouse)
	{
		if (halias.index < 10)
		{
			if (ms_bts[halias.index])
			{
				return 1.0f;
			}
		}

		if (halias.index == 10)
		{
			if (delta)
			{
				if (prev_ms_x == -1000)
				{
					return 0;
				}

				return ms_x - prev_ms_x;
			}

			return ms_x;
		}
		else
		if (halias.index == 11)
		{
			if (delta)
			{
				if (prev_ms_y == -1000)
				{
					return 0;
				}

				return ms_y - prev_ms_y;
			}

			return ms_y;
		}
	}

	return 0.0f;
}

float Controls::GetAliasValue(int index, bool delta)
{
	if (index == -1)
	{
		return 0.0f;
	}

	Alias& alias = aliases[index];

	for (int i = 0; i < alias.aliasesRef.size(); i++)
	{
		AliasRef& aliasRef = alias.aliasesRef[i];

		if (aliasRef.aliasIndex == -1)
		{
			continue;
		}

		float val = 0.0f;

		if (aliasRef.refer2hardware)
		{
			val = GetHardwareAliasValue(aliasRef.aliasIndex, delta);
		}
		else
		{
			val = GetAliasValue(aliasRef.aliasIndex, delta);
		}

		if (fabs(val) > 0.01f)
		{
			return val * aliasRef.modifier;
		}
	}

	return 0.0f;
}

bool Controls::DebugKeyPressed(const char* name, AliasAction action)
{
	if (debeugMap.find(name) == debeugMap.end())
	{
		return false;
	}

	return GetHardwareAliasState(debeugMap[name], false, action);
}

void Controls::OverrideMousePos(int mx, int my)
{
	if (prev_ms_x == -1000)
	{
		prev_ms_x = mx;
		ms_x = mx;

		prev_ms_y = my;
		ms_y = my;
	}

	prev_ms_x = ms_x;
	ms_x = mx;

	prev_ms_y = ms_y;
	ms_y = my;
}

void Controls::Update(float dt)
{
#ifdef PLATFORM_PC
	static byte tmp_diks[256];
	ZeroMemory(tmp_diks, sizeof(tmp_diks));
	HRESULT hr = pKeyboard->GetDeviceState(sizeof(tmp_diks), tmp_diks);

	for (int i = 0; i<256; i++)
	{
		if (tmp_diks[i])
		{
			btns[i]++;

			if (btns[i]>2)
			{
				btns[i] = 2;
			}
		}
		else
		{
			btns[i] = 0;
		}
	}

	if (FAILED(hr))
	{
		hr = pKeyboard->Acquire();

		while (hr == DIERR_INPUTLOST) hr = pKeyboard->Acquire();

		if (hr == DIERR_OTHERAPPHASPRIO || hr == DIERR_NOTACQUIRED)
		{
			return;
		}
	}

	ZeroMemory(&dims2, sizeof(dims2));
	hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &dims2);

	for (int i = 0; i<4; i++)
	{
		if (dims2.rgbButtons[i] & 0x80)
		{
			ms_bts[i]++;

			if (ms_bts[i]>2)
			{
				ms_bts[i] = 2;
			}
		}
		else
		{
			ms_bts[i] = 0;
		}
	}

	if (FAILED(hr))
	{
		hr = pMouse->Acquire();

		while (hr == DIERR_INPUTLOST) hr = pMouse->Acquire();

		return;
	}

	prev_ms_x = ms_x;
	prev_ms_y = ms_y;
#endif
}
