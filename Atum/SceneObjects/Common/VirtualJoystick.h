
#pragma once

#include "Services/Scene/SceneObject.h"
#include "Services/Render/Render.h"

class VirtualJoystick : public SceneObject
{
public:
	META_DATA_DECL(SpriteObject)

	Texture*    tex_stick_pad = nullptr;
	std::string tex_name_stick_pad;

	Texture*    tex_stick = nullptr;
	std::string tex_name_stick;

	Texture*    tex_button_a = nullptr;
	std::string tex_name_button_a;

	Texture*    tex_button_b = nullptr;
	std::string tex_name_button_b;

	Vector2 stick_pad_size;
	Vector2 stick_size;
	Vector2 stick_pos;
	Vector2 stick_delta = 0.0f;
	Vector2 stick_asb_delta = 0.0f;
	Vector2 button_size;
	Vector2 button_a_pos;
	Vector2 button_b_pos;
	bool stick_pressed = false;
	int  stick_alias_index = -1;
	int button_a_pressed = 0;
	int button_a_alias_index = -1;
	int button_b_pressed = 0;
	int button_b_alias_index = -1;

	struct Alias
	{
		int touch = -1;
		int touchX = -1;
		int touchY = -1;
	};

	enum AliasConst
	{
		AliasCount = 4
	};

	Alias aliases[AliasCount];

	VirtualJoystick();
	virtual ~VirtualJoystick();

	void BindClassToScript() override;

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt);
};
