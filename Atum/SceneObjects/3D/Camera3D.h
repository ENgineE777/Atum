
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_3D_Camera3D Camera3D

Currently 3D space is a special mode and this class represents a camera in that special 2D mode.
Camera auto follow a target. Position of a target should be set via script.

This class ::Camera3D is a representation on C++ side.

This class Script::Camera3D is a representation in a script.

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
\ingroup gr_code_scene_objects_3D
*/

/**
\brief Representation of camera in 3D space

Currently 2D space is a special mode and this class represents a camera in that special 2D mode.
Camera auto follow a target. Position of a target should be set via script.

This class Script::Camera3D is a representation in a script.

*/


class Camera3D : public SceneObject
{
public:
	META_DATA_DECL(Camera3D)

	float fov = 45.0f;

	Matrix transform;
	Matrix view;

	float cur_proj_fov = -1.0f;
	Matrix projection;

#ifndef DOXYGEN_SKIP

	Camera3D();
	virtual ~Camera3D();

	void BindClassToScript() override;

	Matrix* Trans() override;
	bool Is3DObject() override;

	void Init() override;
	bool Play() override;
	void Update(float dt);

	void SetPosTarget(Vector3& pos, Vector3& target);
#endif
};
