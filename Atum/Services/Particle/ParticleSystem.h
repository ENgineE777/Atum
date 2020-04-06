
#pragma once

#include "stdio.h"
#include <stdlib.h>
#include <map>
#include <string>
#include "Services/TaskExecutor/TaskExecutor.h"
#include "Spark.h"

class ParticleSystem : public Object
{
	SPK::Ref<SPK::System> res;
	SPK::Ref<SPK::System> system;

	bool playing = true;
	bool visible = true;
	bool autodelete = true;

	Matrix transform;

	TaskExecutor::SingleTaskPool* taskPool;
	TaskExecutor::SingleTaskPool* renderPool;

public:

	void Init(SPK::Ref<SPK::System> res, TaskExecutor::SingleTaskPool* taskPool, TaskExecutor::SingleTaskPool* renderPool, bool autoDelete);

	void SetTransform(Matrix& transform);

	void SetVisible(bool set);
	bool IsVisible();

	void Play(bool set);
	bool IsPlaying();

	void SetAutoDelete(bool set);
	bool IfAutoDelete();

	bool IsSysyemActive();

	void Restart();

	void Update(float dt);
	void Render(float dt);

	void Release();
};
