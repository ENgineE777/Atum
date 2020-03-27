
#pragma once

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"

/**
\page scene_object_3D_Terrain Terrain

Terrain costracted from height map and textured by one big texture. Size is taken from
dimention of a texture. Vertical and horizontal scales can be adjusted.

This class ::Terrain is a representation on C++ side.

Parameters
----------

Name              | Description
------------------| -------------
ScaleH            | Horizontal scale factor
ScaleV            | Vertical scale factor
Texture           | Filename of a texture
Heightmap         | Filename of a high map
Color             | Overlay color of a terrain

*/


/**
\ingroup gr_code_scene_objects_3D
*/

/**
\brief Representation of terrain in 3D space

Terrain costracted from height map and textured by one big texture. Size is taken from
dimention of a texture. Vertical and horizontal scales can be adjusted.

*/

class Terrain : public SceneObject
{
public:
	META_DATA_DECL(Terrain)

	/**
	\brief Filename of a texture.
	*/

	std::string tex_name;

	/**
	\brief Filename of a high map.
	*/
	std::string hgt_name;

	/**
	\brief Horizontal scale factor.
	*/

	float    scaleh;

	/**
	\brief Vertical scale factor.
	*/

	float    scalev;

	/**
	\brief Overlay color of a terrain.
	*/

	Color    color;

#ifndef DOXYGEN_SKIP
	Texture*    texture = nullptr;
	VertexDecl* vdecl;
	DataBuffer* buffer = nullptr;
	int      hwidth;
	int      hheight;
	uint8_t* hmap = nullptr;

	struct VertexTri
	{
		Vector3 position;
		Vector2 texCoord;
		Vector3 normal;
	};

	int sz;

	PhysScene::BodyUserData body;

	Terrain();
	virtual ~Terrain();

	bool Is3DObject() override;
	void Init() override;
	void ApplyProperties() override;
	float GetHeight(int i, int j);
	Vector3 GetVecHeight(int i, int j);
	void LoadHMap(const char* hgt_name);
	void Render(float dt);
	void ShRender(float dt);
	void Render(Program* prg);

	bool Play() override;

	void Release() override;
#endif
};
