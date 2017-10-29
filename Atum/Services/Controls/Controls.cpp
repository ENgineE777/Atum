
#include "Controls.h"
#include "Services/Render/Render.h"
#include "Support/json/JSONReader.h"


Controls controls;

bool Controls::Init(const char* name_haliases, bool allowDebugKeys)
{
	this->allowDebugKeys = allowDebugKeys;

#ifdef PLATFORM_PC
	HWND hwnd = NULL;

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

	pKeyboard->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

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

	pMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

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

			debeugMap[halias.name] = (int)haliases.size() - 1;

			reader->LeaveBlock();
		}

		while (reader->EnterBlock("mouse"))
		{
			haliases.push_back(HardwareAlias());
			HardwareAlias& halias = haliases[(int)haliases.size() - 1];

			halias.device = Mouse;
			reader->Read("name", halias.name);
			reader->Read("index", halias.index);

			debeugMap[halias.name] = (int)haliases.size() - 1;

			reader->LeaveBlock();
		}
	}

	reader->Release();

	return true;
}

bool Controls::LoadAliases(const char* name_aliases)
{
	JSONReader* reader = new JSONReader();

	bool res = false;

	if (reader->Parse(name_aliases))
	{
		res = true;

		while (reader->EnterBlock("Aliases"))
		{
			aliases.push_back(Alias());
			Alias& alias = aliases.back();

			reader->Read("name", alias.name);

			while (reader->EnterBlock("AliasesRef"))
			{
				alias.aliasesRef.push_back(AliasRef());
				AliasRef& aliasRef = alias.aliasesRef.back();

				while (reader->EnterBlock("names"))
				{
					aliasRef.refs.push_back(AliasRefState());
					AliasRefState& ref = aliasRef.refs.back();

					reader->Read("", ref.name);

					reader->LeaveBlock();
				}

				reader->Read("modifier", aliasRef.modifier);

				reader->LeaveBlock();
			}

			reader->LeaveBlock();
		}

		ResolveAliases();
	}

	reader->Release();

	return res;
}

void Controls::ResolveAliases()
{
	for (auto&  alias : aliases)
	{
		for (auto&  aliasRef : alias.aliasesRef)
		{
			for (auto& ref : aliasRef.refs)
			{
				int index = GetAlias(ref.name.c_str());

				if (index != -1)
				{
					ref.aliasIndex = index;
					ref.refer2hardware = false;
				}
				else
				{
					for (int l = 0; l < haliases.size(); l++)
					{
						if (StringUtils::IsEqual(haliases[l].name.c_str(), ref.name.c_str()))
						{
							ref.aliasIndex = l;
							ref.refer2hardware = true;
							break;
						}
					}
				}
			}
		}
	}

	for (auto&  alias : aliases)
	{
		CheckDeadEnds(alias);
	}
}

void Controls::CheckDeadEnds(Alias& alias)
{
	alias.visited = true;

	for (auto& aliasRef : alias.aliasesRef)
	{
		for (auto& ref : aliasRef.refs)
		{
			if (ref.aliasIndex != -1 && !ref.refer2hardware)
			{
				if (aliases[ref.aliasIndex].visited)
				{
					ref.aliasIndex = -1;
				}
				else
				{
					CheckDeadEnds(aliases[ref.aliasIndex]);
				}
			}
		}
	}

	alias.visited = false;
}

void Controls::SetWindow(void* wnd)
{
	hwnd = *((HWND*)wnd);
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

bool Controls::GetHardwareAliasState(int index, AliasAction action)
{
	HardwareAlias& halias = haliases[index];

	if (halias.device == Keyboard)
	{
		if (action == Activated)
		{
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

bool Controls::GetAliasState(int index, AliasAction action)
{
	if (index == -1 || index >= aliases.size())
	{
		return 0.0f;
	}

	Alias& alias = aliases[index];

	for (auto& aliasRef : alias.aliasesRef)
	{
		bool val = true;

		for (auto& ref : aliasRef.refs)
		{
			if (ref.aliasIndex == -1)
			{
				continue;
			}

			if (ref.refer2hardware)
			{
				val &= GetHardwareAliasState(ref.aliasIndex, Active);
			}
			else
			{
				val &= GetAliasState(ref.aliasIndex, Active);
			}
		}

		if (action == Activated && val)
		{
			val = false;

			for (auto& ref : aliasRef.refs)
			{
				if (ref.aliasIndex == -1)
				{
					continue;
				}

				if (ref.refer2hardware)
				{
					val |= GetHardwareAliasState(ref.aliasIndex, Activated);
				}
				else
				{
					val |= GetAliasState(ref.aliasIndex, Activated);
				}
			}
		}

		if (val)
		{
			return true;
		}
	}

	return false;
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

				return (float)(ms_x - prev_ms_x);
			}

			return (float)ms_x;
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

				return (float)(ms_y - prev_ms_y);
			}

			return (float)ms_y;
		}
	}

	return 0.0f;
}

float Controls::GetAliasValue(int index, bool delta)
{
	if (index == -1 || index >= aliases.size())
	{
		return 0.0f;
	}

	Alias& alias = aliases[index];

	for (auto& aliasRef : alias.aliasesRef)
	{
		float val = 0.0f;

		for (auto& ref : aliasRef.refs)
		{
			if (ref.aliasIndex == -1)
			{
				continue;
			}

			if (ref.refer2hardware)
			{
				val = GetHardwareAliasValue(ref.aliasIndex, delta);
			}
			else
			{
				val = GetAliasValue(ref.aliasIndex, delta);
			}
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
	if (!allowDebugKeys || !name)
	{
		return false;
	}

	if (debeugMap.find(name) == debeugMap.end())
	{
		return false;
	}

	return GetHardwareAliasState(debeugMap[name], action);
}

bool Controls::DebugHotKeyPressed(const char* name, const char* name2, const char* name3)
{
	if (!allowDebugKeys)
	{
		return false;
	}

	bool active = DebugKeyPressed(name, Active) & DebugKeyPressed(name2, Active);

	if (name3)
	{
		active &= DebugKeyPressed(name3, Active);
	}

	if (active)
	{
		if (DebugKeyPressed(name) | DebugKeyPressed(name2) | DebugKeyPressed(name3))
		{
			return true;
		}
	}

	return false;
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
	if (GetFocus() != hwnd)
	{
		ZeroMemory(btns, 256);
		ZeroMemory(ms_bts, 10);

		return;
	}

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

		return;
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

		return;
	}

	prev_ms_x = ms_x;
	prev_ms_y = ms_y;
#endif
}
