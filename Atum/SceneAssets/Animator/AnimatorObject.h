
#pragma once

#include "Services/Scene/SceneObject.h"
#include "Animator.h"

class AnimatorObject : public SceneObject
{
public:
	META_DATA_DECL(AnimatorObject)

	string animator_name;
	bool isLooped;
	float curTime = 0.0f;

	Animator* animator = nullptr;

	CLASSDECLDIF(SceneObject, AnimatorObject)
	AnimatorObject();
	virtual ~AnimatorObject();

	virtual void Init();
	void Work(float dt);

	void Play();
	void Stop();
};
