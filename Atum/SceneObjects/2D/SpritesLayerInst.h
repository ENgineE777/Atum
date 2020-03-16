
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpritesLayerAsset.h"

/**
\page scene_assets_2D_SpritesLayerInst Instance

Instance automaticaly wraps layer in horizontal direction. Depth and axis scale are
controlable. Severel layers with differetes axis sclae factor allows to create paralax effect.

![Caption text](../images/SpriteLayer.jpg "Image")

This class ::SpritesLayerInst is a representation on C++ side.

Parameters
----------

Name              | Description
------------------| -------------
x_axis_scale      | X axis
y_axis_scale      | Y axis
Depth             | Z depth
y                 | Y offset

*/

/**
\ingroup gr_code_scene_objects_2D
*/

/**
\brief Representation of instance of sprite layer

Instance automaticaly wraps layer in horizontal direction. Depth and axis scale are
controlable. Severel layers with differetes axis sclae factor allows to create paralax effect.

*/

class SpritesLayerInst : public SceneObjectInst
{
public:
	META_DATA_DECL(SpritesLayerInst)

	GET_ASSET_METHOD(SpritesLayerAsset)

	/**
	\brief Axis scale of a layer
	*/

	Vector2 axis_scale = 1.0f;

#ifndef DOXYGEN_SKIP
	Transform2D trans;

	virtual ~SpritesLayerInst() = default;

	void Init() override;
	void Draw(float dt);
#endif
};
