
#pragma once

#include "Services/Scene/SceneObject.h"
#include "Services/Render/Render.h"

/**
\page scene_object_common_VirtualJoystick Virtual Joystick

On screen virtual joystick can be used in script and in another scene object as source of input.
Joystick consist from one virtual stick and two buttons.

This class ::VirtualJoystick is a representation on C++ side.

This class Script::VirtualJoystick is a representation in a script.

Parameters
----------

Name              | Description
------------------| -------------
StickPad          | Filename of a stick pad texture
Stick             | Filename of a stick texture
ButtonA           | Filename of first button texture
ButtonB           | Filename of second button texture

*/


/**
\ingroup gr_code_scene_objects_common
*/

/**
\brief Representation of on screen virutal joystick

On screen virtual joystick can be used in script and in another scene object as source of input.
Joystick consist from one virtual stick and two buttons.

This class Script::VirtualJoystick is a representation in a script.

*/

class VirtualJoystick : public SceneObject
{
public:
	META_DATA_DECL(SpriteObject)

	/**
	\brief Filename of a stick pad texture.
	*/

	std::string tex_name_stick_pad;

	/**
	\brief Filename of a stick texture.
	*/

	std::string tex_name_stick;
	/**
	\brief Filename of first button texture.
	*/

	std::string tex_name_button_a;

	/**
	\brief Filename of second button texture.
	*/

	std::string tex_name_button_b;

	/**
	\brief Axis state of a stick.
	*/

	Vector2 stick_delta = 0.0f;

	/**
	\brief State of a button. 1 means pressed. 0 means unpressed.
	*/

	int button_a_pressed = 0;

	/**
	\brief State of b button. 1 means pressed. 0 means unpressed.
	*/

	int button_b_pressed = 0;

#ifndef DOXYGEN_SKIP

	Texture*    tex_stick = nullptr;
	Texture*    tex_stick_pad = nullptr;
	Texture*    tex_button_a = nullptr;
	Texture*    tex_button_b = nullptr;

	Vector2 stick_pad_size;
	Vector2 stick_size;
	Vector2 stick_pos;
	Vector2 stick_asb_delta = 0.0f;
	Vector2 button_size;
	Vector2 button_a_pos;
	Vector2 button_b_pos;
	bool stick_pressed = false;
	int  stick_alias_index = -1;
	int button_a_alias_index = -1;
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
#endif
};
