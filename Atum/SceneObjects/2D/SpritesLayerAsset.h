
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

/**
\page scene_assets_2D_SpritesLayerAsset Asset

This asset holds array of sprites that later can be used as a layer.

![Caption text](../images/SpriteLayer.jpg "Image")

This class ::SpritesLayerAsset is a representation on C++ side.

Script property parameters
--------------------------

Name              | Description
------------------| -------------
x                 | X coordinate of a camera position
y                 | Y coordinate of a camera position
width             | Width of a sprite
height            | Height of a sprite
sprite            | Data of a sprite

Parameters
----------

Name              | Description
------------------| -------------
sprites           | Array of sprites

*/

/**
\ingroup gr_code_scene_assets_2D
*/

/**
\brief Representation of layer consisted from 2D sprites

This class holds array of sprites that later can be used as a layer.

*/

class SpritesLayerAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpritesLayerAsset)

#ifndef DOXYGEN_SKIP

	struct LayerSprite
	{
		META_DATA_DECL_BASE(Sprite)

		Vector2 pos;
		Vector2 size;
		Sprite::Data sprite;
		Sprite::FrameState state;
	};

	int sel_sprite = -1;
	vector<LayerSprite> sprites;

	SpritesLayerAsset();
	virtual ~SpritesLayerAsset() = default;

	void Init() override;
	void Draw(float dt);

#ifdef EDITOR
	void SetEditMode(bool ed) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void SetGizmo();
#endif

#endif
};
