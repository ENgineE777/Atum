
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_2D_Camera2D Camera2D

Currently 2D space is a special mode and this class represents a camera in that special 2D mode.
Camera auto follow a target. Position of a target should be set via script.

This class ::Camera2D is a representation on C++ side.

This class Script::Camera2D is a representation in a script.

Parameters
----------

Name              | Description
------------------|--------------
x                 | X coordinate of a camera position
y                 | Y coordinate of a camera position
ScreenBorder      | Size of border at left and right of the screen where camera is starting to follow object
ScreenVertBorder  | Size of border at top and bottom of the screen where camera is starting to follow object
UseLRBorder       | This parameter controls useage of left and right border which limits movement of the camera
LeftBorder        | This parameter set up left border after which camera will not follow a target
RightBorder       | This parameter set up right border after which camera will not follow a target
*/

/**
\ingroup gr_code_scene_objects_2D
*/

/**
\brief Representation of camera in 2D space

Currently 2D space is a special mode and this class represents a camera in that special 2D mode.
Camera auto follow a target. Position of a target should be set via script.

This class Script::Camera2D is a representation in a script.

*/


class Camera2D : public SceneObject
{
public:
	META_DATA_DECL(Camera2D)

	/**
	\brief position of a target
	*/

	Vector2 target_pos = 0.0f;

	/**
	\brief Transform of a camera
	*/

	Transform2D trans;

	/**
	\brief Size of border at left and right of the screen where camera is starting to follow object 
	*/
	float screen_border = 100.0f;

	/**
	\brief Size of border at top and bottom of the screen where camera is starting to follow object
	*/
	float screen_vert_border = 100.0f;

	/**
	\brief This parameter set up left border after which camera will not follow a target
	*/
	float left_border = -300.0f;

	/**
	\brief This parameter set up right border after which camera will not follow a target
	*/
	float right_border =300.0f;

	/**
	\brief This parameter controls useage of left and right border which limits movement of the camera
	*/
	bool use_lr_borders = false;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	Vector2 init_pos;

#ifndef DOXYGEN_SKIP

	Camera2D();
	virtual ~Camera2D();

	void BindClassToScript() override;

	void Init() override;
	void Update(float dt);

	bool Play() override;
	void Stop() override;

	void Reset();

#ifdef EDITOR
	void Set2DPos(Vector2 pos) override;
	void SetEditMode(bool ed);
#endif

#endif
};
