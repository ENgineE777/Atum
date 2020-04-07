
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

	bool simulating = true;
	bool emitersActive = true;
	bool visible = true;
	bool autoDelete = true;

	Matrix transform;

	TaskExecutor::SingleTaskPool* taskPool;
	TaskExecutor::SingleTaskPool* renderPool;

public:

	void Init(SPK::Ref<SPK::System> res, TaskExecutor::SingleTaskPool* taskPool, TaskExecutor::SingleTaskPool* renderPool, bool autoDelete);

	void SetTransform(Matrix& transform);

	void SetVisible(bool set);
	bool IsVisible();

	void SetSimulate(bool set);
	bool IsSimulating();

	void SetEmitersActive(bool set);
	bool IsEmitersActive();

	void SetAutoDelete(bool set);
	bool IfAutoDelete();

	bool IsSysyemActive();

	void Restart();

	void Update(float dt);
	void Render(float dt);

	void Release();
};
