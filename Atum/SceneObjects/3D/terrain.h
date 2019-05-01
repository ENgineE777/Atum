
#pragma once

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"

/**
\page scene_object_3D_Terrain Terrain

Terrain

*/


/**
\ingroup gr_code_scene_objects_3D
*/

class Terrain : public SceneObject
{
public:
	META_DATA_DECL(Terrain)

	VertexDecl*     vdecl;
	GeometryBuffer* buffer = nullptr;
	Texture*        texture = nullptr;
	int      hwidth;
	int      hheight;
	float    scaleh;
	float    scalev;
	Color    color;
	uint8_t* hmap = NULL;
	std::string tex_name;
	std::string hgt_name;

	struct VertexTri
	{
		Vector  position;
		Vector2 texCoord;
		Vector  normal;
	};

	int sz;

	PhysHeightmap*  hm;

	Terrain();
	virtual ~Terrain();

	bool Is3DObject() override;
	virtual void Init();
	virtual void ApplyProperties();
	float GetHeight(int i, int j);
	Vector GetVecHeight(int i, int j);
	void LoadHMap(const char* hgt_name);
	void Render(float dt);
	void ShRender(float dt);
	void Render(Program* prg);

	bool Play();
	void Stop();
};
