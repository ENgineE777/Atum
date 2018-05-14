
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteGraphAsset.h"
#include "SimpleCharacter2D.h"

class CharacterCamera2D : public SceneObject
{
public:
	META_DATA_DECL(CharacterCamera2D)

	SimpleCharacter2D* target = nullptr;
	string target_name; 
	Transform2D trans;
	float screen_border = 100.0f;
	float left_border = -300.0f;
	float right_border =300.0f; 
	
	CLASSDECLDIF(SceneObject, CharacterCamera2D)
	CharacterCamera2D();
	virtual ~CharacterCamera2D();

	virtual void Init();
	void Update(float dt);

	void Play();
	void Stop();

#ifdef EDITOR
	void SetEditMode(bool ed);
#endif
};
