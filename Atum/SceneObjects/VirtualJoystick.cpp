
#include "VirtualJoystick.h"
#include "SceneAssets/Sprite.h"
#include "Services/Core/Core.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, VirtualJoystick, "VirtualJoystick")

META_DATA_DESC(VirtualJoystick)
FILENAME_PROP(VirtualJoystick, tex_name_stick_pad, "", "Properties", "StickPad")
FILENAME_PROP(VirtualJoystick, tex_name_stick, "", "Properties", "Stick")
FILENAME_PROP(VirtualJoystick, tex_name_button_a, "", "Properties", "ButtonA")
FILENAME_PROP(VirtualJoystick, tex_name_button_b, "", "Properties", "ButtonB")
META_DATA_DESC_END()

VirtualJoystick::VirtualJoystick() : SceneObject()
{
}

VirtualJoystick::~VirtualJoystick()
{
}

void VirtualJoystick::Init()
{
	char str[128];
	for (int i = 0; i < AliasCount; i++)
	{
		Alias& alias = aliases[i];
		StringUtils::Printf(str, 128, "VJoystick.Touch%i", i);
		alias.touch = controls.GetAlias(str);
		StringUtils::Printf(str, 128, "VJoystick.TouchX%i", i);
		alias.touchX = controls.GetAlias(str);
		StringUtils::Printf(str, 128, "VJoystick.TouchY%i", i);
		alias.touchY = controls.GetAlias(str);
	}

	RenderTasks(false)->AddTask(RenderLevels::GUI, this, (Object::Delegate)&VirtualJoystick::Draw);
}

void VirtualJoystick::ApplyProperties()
{
	RELEASE(tex_stick_pad)
	tex_stick_pad = render.LoadTexture(tex_name_stick_pad.c_str());

	RELEASE(tex_stick)
	tex_stick = render.LoadTexture(tex_name_stick.c_str());

	RELEASE(tex_button_a)
	tex_button_a = render.LoadTexture(tex_name_button_a.c_str());

	RELEASE(tex_button_b)
	tex_button_b = render.LoadTexture(tex_name_button_b.c_str());

	stick_pos = 200.0f;
	stick_pad_size = 250.0f;
	stick_size = 120.0f;

	button_size = 100.0f;
	button_a_pos = 100.0f;
	button_b_pos = Vector2(100.0f, 250.0f);
}

void VirtualJoystick::Draw(float dt)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;


	{
		if (stick_pressed)
		{
			Vector2 delta_ms(controls.GetAliasValue(aliases[stick_alias_index].touchX, true), controls.GetAliasValue(aliases[stick_alias_index].touchY, true));
			core.Log("DELTA", "delta_ms %4.3f %4.3f", delta_ms.x, delta_ms.y);
			delta_ms /= scale;
			stick_delta += delta_ms;
		}

		if (stick_pressed && !controls.GetAliasState(aliases[stick_alias_index].touch, Controls::Active))
		{
			stick_pressed = false;
			stick_alias_index = -1;
			stick_delta = 0.0f;
			stick_asb_delta = 0.0f;
		}

		if (button_a_pressed > 0 && !controls.GetAliasState(aliases[button_a_alias_index].touch, Controls::Active))
		{
			button_a_pressed = 0;
			button_a_alias_index = -1;
		}

		if (button_b_pressed > 0 && !controls.GetAliasState(aliases[button_b_alias_index].touch, Controls::Active))
		{
			button_b_pressed = 0;
			button_b_alias_index = -1;
		}
	}

	for (int i = 0; i < AliasCount; i++)
	{
		Alias& alias = aliases[i];

		if (controls.GetAliasState(alias.touch))
		{
			Vector2 ms(controls.GetAliasValue(alias.touchX, false), controls.GetAliasValue(alias.touchY, false));
			ms /= scale;
			float scr_width = (float)render.GetDevice()->GetWidth() / scale;

			if (!stick_pressed &&
				0.0f < ms.x && ms.x < 0.5f * (float)render.GetDevice()->GetWidth() / scale &&
				0.5f * 1024.0f < ms.y && ms.y < 1024.0f)
			{
				stick_pos.x = ms.x;
				stick_pos.y = 1024.0f - ms.y;
				stick_alias_index = i;
				stick_pressed = true;
			}
			else
			if (button_a_pressed == 0 &&
				scr_width - button_a_pos.x - button_size.x * 0.5f < ms.x && ms.x < scr_width - button_a_pos.x + button_size.x * 0.5f &&
				1024.0f - button_a_pos.y - button_size.y * 0.5f < ms.y && ms.y < 1024.0f - button_a_pos.y + button_size.y * 0.5f)
			{
				button_a_pressed = 1;
				button_a_alias_index = i;
			}
			else
			if (button_b_pressed == 0 &&
				scr_width - button_b_pos.x - button_size.x * 0.5f < ms.x && ms.x < scr_width - button_b_pos.x + button_size.x * 0.5f &&
				1024.0f - button_b_pos.y - button_size.y * 0.5f < ms.y && ms.y < 1024.0f - button_b_pos.y + button_size.y * 0.5f)
			{
				button_b_pressed = 1;
				button_b_alias_index = i;
			}
		}
	}

	Vector2 pos(stick_pos.x, 1024.0f - stick_pos.y);

	Color clr = COLOR_WHITE;

	clr.a = stick_pressed ? 1.0f : 0.5f;
	Matrix mat;

	Vector2 delta = 0.0f;

	if (stick_pressed)
	{
		delta = stick_delta;
		float len = delta.Length();

		if (len > stick_pad_size.x * 0.5f)
		{
			delta.Normalize();
			delta *= stick_pad_size.x * 0.5f;
			len = stick_pad_size.x * 0.5f;
		}

		stick_asb_delta = stick_delta;
		stick_asb_delta.Normalize();
		stick_asb_delta *= len / (stick_pad_size.x * 0.5f);
		stick_asb_delta.y = -stick_asb_delta.y;
	}

	mat.Pos().y = 0.5f;
	Sprite::Draw(tex_stick, clr, mat, (pos - stick_size * 0.5 + delta) * scale, stick_size * scale, 0.0f, 1.0f, false);

	mat.Pos().y = 0.45f;
	Sprite::Draw(tex_stick_pad, clr, mat, (pos - stick_pad_size * 0.5) * scale, stick_pad_size * scale, 0.0f, 1.0f, false);

	if (tex_button_a)
	{
		clr.a = (button_a_pressed > 0) ? 1.0f : 0.5f;
		Sprite::Draw(tex_button_a, clr, mat, Vector2(render.GetDevice()->GetWidth() - button_a_pos.x * scale, render.GetDevice()->GetHeight() - button_a_pos.y * scale) - button_size * 0.5 * scale, button_size * scale, 0.0f, 1.0f, false);

		if (button_a_pressed != 0 && button_a_pressed < 3)
		{
			button_a_pressed++;
		}
	}

	if (tex_button_b)
	{
		clr.a = (button_b_pressed > 0) ? 1.0f : 0.5f;
		Sprite::Draw(tex_button_b, clr, mat, Vector2(render.GetDevice()->GetWidth() - button_b_pos.x * scale, render.GetDevice()->GetHeight() - button_b_pos.y * scale) - button_size * 0.5 * scale, button_size * scale, 0.0f, 1.0f, false);

		if (button_b_pressed != 0 && button_b_pressed < 3)
		{
			button_b_pressed++;
		}
	}
}