
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

/**
\page scene_assets_2D_SpriteTileAsset Asset

This assest allows to set nine tiles which will be auto used for instances. This allows easelly create tiled blocks.

![Caption text](../images/NineTiles.jpg "Image")

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
spriteWN          | Top left sprite data
spriteN           | Top sprite data
spriteEN          | Top right sprite data
spriteW           | Left sprite data
spriteC           | Center sprite data
spriteE           | Right sprite data
spriteWS          | Bottom left sprite data
spriteS           | Bottom sprite data
spriteES          | Bottom right sprite data

*/


/**
\ingroup gr_code_scene_assets_2D
*/

/**
\brief Representation of nine tiled 2D sprite

This class holds array of nine 2D sprites. Proper tile from set of nine is choosing automaticaly.

*/

class SpriteTileAsset : public SceneAsset
{
public:

#ifndef DOXYGEN_SKIP

	META_DATA_DECL(SpriteTileAsset)

	Transform2D trans;

	Sprite::Data sprite[9];
	static Sprite::FrameState state[9];

	SpriteTileAsset();
	virtual ~SpriteTileAsset() = default;

	void Init() override;
	void Draw(float dt);

#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif

#endif
};
