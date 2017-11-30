
#pragma once

#include "Services/Scene/SceneObject.h"
#include "quad.h"

class TDPrt : public SceneObject
{
public:
	META_DATA_DECL(TDPrt)

	float sizeX;
	float sizeY;
	float sizeZ;
	bool  isStatic;
	Color color;

	struct Enemy
	{
		float anim = 0.0f;
	};

	Enemy enemy;

	std::vector<Texture*> textures;

	Quad quad;

	Texture* map_tex;
	Texture* tower_tex;

	Texture* enemy_tex[3];

	CLASSDECLDIF(SceneObject, TDPrt)
	TDPrt();
	virtual ~TDPrt();

	virtual void Init();
	void Draw(float dt);

	void Play();
	void Stop();
};
