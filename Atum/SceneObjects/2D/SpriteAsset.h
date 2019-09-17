
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

/**
\page scene_assets_2D_SpriteAsset Asset

This asset stores 2D sprite data.

![Caption text](../images/Sprites.jpg "Image")

This class ::SpriteAsset is a representation on C++ side.

Parameters
----------

Name              | Description
------------------|--------------
x                 | X coordinate of a camera position
y                 | Y coordinate of a camera position
width             | Width of a sprite
height            | Height of a sprite
anchorn_x         | X coordinate of anchorn in absolute units
anchorn_y         | Y coordinate of anchorn in absolute units
sprite            | Sprite data

*/


/**
\ingroup gr_code_scene_assets_2D
*/

/**
\brief Representation of 2D sprite asset

This class stores 2D sprite data of asset.

*/

class SpriteAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpriteAsset)

	bool use_source_size = false;
	Vector2 source_scale;

	/**
	\brief Transform data of a sprite. This data will be used in instances of a sprite.
	*/

	Transform2D trans;

	/**
	\brief Data of a sprite.
	*/
	Sprite::Data sprite;

#ifndef DOXYGEN_SKIP
	static Sprite::FrameState state;

	SpriteAsset();
	virtual ~SpriteAsset() = default;

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt);

#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif

#endif
};
