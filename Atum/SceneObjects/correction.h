
#pragma once

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"

class ColorCorrection : public SceneObject
{
public:

	META_DATA_DECL(ColorCorrection)

	Program*        color_prg = nullptr;
	Program*        blur_prg = nullptr;
	Program*        combine_prg = nullptr;
	VertexDecl*     vdecl = nullptr;
	GeometryBuffer* buffer = nullptr;
	Texture*        ring_rt[2];

	Texture* scene_rt = nullptr;
	Texture* scene_depth = nullptr;

	virtual void Init();

	void ReCreateRT();
	float ComputeGaussian(float n);

	void SetRT(float dt);
	void Draw(float dt);

	virtual void Release();
};
