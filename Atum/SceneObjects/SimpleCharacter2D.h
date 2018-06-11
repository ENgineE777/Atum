
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteGraphAsset.h"
#include "VirtualJoystick.h"

class SimpleCharacter2D : public SceneObject
{
public:
	META_DATA_DECL(SimpleCharacter2D)

	VirtualJoystick* vjoy = nullptr;
	string vjoy_name;

	SpriteGraphAsset* asset = nullptr;
	SpriteGraphAsset::Instance graph_instance;
	SimpleCharacter2D* target = nullptr;
	string asset_name; 
	Transform2D trans;
	bool allow_move = false;
	float speed = 120.0f;
	float floor_height = 100.0f;
	bool is_enemy = true;
	bool flipped = false;
	Vector2 floor_margin;
	int dir_horz = 0;
	int dir_vert = 0;
	float time_2_kick = 0.5f;
	float cur_time_2_kuck = 0;
	float cur_time_to_kick = -1.0f;
	float death_fly = -1.0f;
	float vanish_time = -1.0f;
	float arraive = -1.0f;
	float resp_time = -1.0f;
	int max_hp = 100;
	int cur_hp = 100;
	int next_kick = -1;

	SimpleCharacter2D();
	virtual ~SimpleCharacter2D();

	virtual void Init();
	virtual void ApplyProperties();
	void Update(float dt);
	void Draw(float dt);

	void Play();
	void Stop();

	SimpleCharacter2D* FindTarget();
	void ControlPlayer(float dt);
	void ControlEnemy(float dt);
	void MakeHit(Vector2 pos, int damage);
	void Respawn();

#ifdef EDITOR
	void SetEditMode(bool ed);
#endif
};
