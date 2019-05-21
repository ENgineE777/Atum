
#include "VirtualJoystick.h"
#include "SceneObjects/2D/Sprite.h"
#include "Services/Core/Core.h"

CLASSREG(SceneObject, VirtualJoystick, "VirtualJoystick")

META_DATA_DESC(VirtualJoystick)
BASE_SCENE_OBJ_PROP(VirtualJoystick)
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

void VirtualJoystick::BindClassToScript()
{
	const char* brief = "Representation of on screen virtual joystick\n"
		"\n"
		"On screen virtual joystick can be used in script and in another scene object as source of input.\n"
		"Joystick consist from one virtual stick and two buttons.\n"
		"\n"
		"This class ::VirtualJoystick is a representation on C++ side.\n";

	BIND_TYPE_TO_SCRIPT(VirtualJoystick, brief)
	core.scripts.RegisterObjectProperty(script_class_name, "float stick_delta_x", memberOFFSET(VirtualJoystick, stick_delta.x), "X-axis state of a stick.");
	core.scripts.RegisterObjectProperty(script_class_name, "float stick_delta_y", memberOFFSET(VirtualJoystick, stick_delta.y), "Y-axis state of a stick.");
	core.scripts.RegisterObjectProperty(script_class_name, "int button_a_pressed", memberOFFSET(VirtualJoystick, button_a_pressed), "State of a button. 1 means pressed. 0 means unpressed.");
	core.scripts.RegisterObjectProperty(script_class_name, "int button_b_pressed", memberOFFSET(VirtualJoystick, button_b_pressed), "State of b button. 1 means pressed. 0 means unpressed.");
}

void VirtualJoystick::Init()
{
	char str[128];
	for (int i = 0; i < AliasCount; i++)
	{
		Alias& alias = aliases[i];
		StringUtils::Printf(str, 128, "VJoystick.Touch%i", i);
		alias.touch = core.controls.GetAlias(str);
		StringUtils::Printf(str, 128, "VJoystick.TouchX%i", i);
		alias.touchX = core.controls.GetAlias(str);
		StringUtils::Printf(str, 128, "VJoystick.TouchY%i", i);
		alias.touchY = core.controls.GetAlias(str);
	}

	RenderTasks(false)->AddTask(ExecuteLevels::GUI, this, (Object::Delegate)&VirtualJoystick::Draw);
}

void VirtualJoystick::ApplyProperties()
{
	RELEASE(tex_stick_pad)
	tex_stick_pad = core.render.LoadTexture(tex_name_stick_pad.c_str());

	RELEASE(tex_stick)
	tex_stick = core.render.LoadTexture(tex_name_stick.c_str());

	RELEASE(tex_button_a)
	tex_button_a = core.render.LoadTexture(tex_name_button_a.c_str());

	RELEASE(tex_button_b)
	tex_button_b = core.render.LoadTexture(tex_name_button_b.c_str());

	stick_pos = 200.0f;
	stick_pad_size = 250.0f;
	stick_size = 120.0f;

	button_size = 175.0f;
	button_a_pos = 125.0f;
	button_b_pos = Vector2(125.0f, 350.0f);
}

void VirtualJoystick::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

	if (GetScene()->Playing())
	{
		if (stick_pressed)
		{
			Vector2 delta_ms(core.controls.GetAliasValue(aliases[stick_alias_index].touchX, true), core.controls.GetAliasValue(aliases[stick_alias_index].touchY, true));
			delta_ms /= scale;
			stick_delta += delta_ms;
		}

		if (GetState() == Inactive || (stick_pressed && !core.controls.GetAliasState(aliases[stick_alias_index].touch, Controls::Active)))
		{
			stick_pressed = false;
			stick_alias_index = -1;
			stick_delta = 0.0f;
			stick_asb_delta = 0.0f;
		}

		if (GetState() == Inactive || (button_a_pressed > 0 && !core.controls.GetAliasState(aliases[button_a_alias_index].touch, Controls::Active)))
		{
			button_a_pressed = 0;
			button_a_alias_index = -1;
		}

		if (GetState() == Inactive || (button_b_pressed > 0 && !core.controls.GetAliasState(aliases[button_b_alias_index].touch, Controls::Active)))
		{
			button_b_pressed = 0;
			button_b_alias_index = -1;
		}
	}

	if (GetScene()->Playing() && GetState() == Active)
	{
		for (int i = 0; i < AliasCount; i++)
		{
			Alias& alias = aliases[i];

			if (core.controls.GetAliasState(alias.touch))
			{
				Vector2 ms(core.controls.GetAliasValue(alias.touchX, false), core.controls.GetAliasValue(alias.touchY, false));
				ms /= scale;
				float scr_width = (float)core.render.GetDevice()->GetWidth() / scale;

				if (!stick_pressed &&
					0.0f < ms.x && ms.x < 0.5f * (float)core.render.GetDevice()->GetWidth() / scale &&
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
		Sprite::Draw(tex_button_a, clr, mat, Vector2(core.render.GetDevice()->GetWidth() - button_a_pos.x * scale, core.render.GetDevice()->GetHeight() - button_a_pos.y * scale) - button_size * 0.5 * scale, button_size * scale, 0.0f, 1.0f, false);

		if (button_a_pressed != 0 && button_a_pressed < 3)
		{
			button_a_pressed++;
		}
	}

	if (tex_button_b)
	{
		clr.a = (button_b_pressed > 0) ? 1.0f : 0.5f;
		Sprite::Draw(tex_button_b, clr, mat, Vector2(core.render.GetDevice()->GetWidth() - button_b_pos.x * scale, core.render.GetDevice()->GetHeight() - button_b_pos.y * scale) - button_size * 0.5 * scale, button_size * scale, 0.0f, 1.0f, false);

		if (button_b_pressed != 0 && button_b_pressed < 3)
		{
			button_b_pressed++;
		}
	}
}