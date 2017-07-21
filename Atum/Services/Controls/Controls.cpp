
#include "Controls.h"
#include "Services/Render/Render.h"


Controls controls;

bool Controls::Init(void* data)
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

	render.AddDelegate("controls", this, (Object::Delegate)&Controls::Update, 0);

	return true;
}

int Controls::GetAlias(const char* name)
{
	return -1;
}

bool Controls::GetAliasState(int alias, bool exclusive, AliasAction action)
{
#ifdef PLATFORM_PC
	HWND active_window = ::GetActiveWindow();
	HWND focus_window = ::GetFocus();

	if (!active_window)
	{
		return false;
	}

	if (alias >= 1000)
	{
		int ms_index = alias - 1000;

		if (action == Activated)
		{
			if (exclusive && ms_bts[ms_index] == 1)
			{
				ms_bts[ms_index]++;
				return true;
			}

			return (ms_bts[ms_index] == 1);
		}

		if (action == Active)
		{
			return (ms_bts[ms_index] > 0);
		}

		return false;
	}
#endif

	if (action == Activated)
	{
		if (exclusive && btns[alias] == 1)
		{
			btns[alias]++;
			return true;
		}

		return (btns[alias] == 1);
	}

	if (action == Active)
	{
		return (btns[alias]>0);
	}

	return false;
}

float Controls::GetAliasValue(int alias, bool delta)
{
	if (alias == 1500)
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

	if (alias == 1501)
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

	return 0.0f;
}

void Controls::SetAliasValue(int alias, float value)
{
	if (alias == 1500)
	{
		if (prev_ms_x == -1000)
		{
			prev_ms_x = value;
			ms_x = value;

			return;
		}

		prev_ms_x = ms_x;
		ms_x = value;
	}

	if (alias == 1501)
	{
		if (prev_ms_y == -1000)
		{
			prev_ms_y = value;
			ms_y = value;

			return;
		}

		prev_ms_y = ms_y;
		ms_y = value;
	}
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
