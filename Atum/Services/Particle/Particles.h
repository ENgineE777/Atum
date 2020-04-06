
#pragma once

#include "Services/Core/Core.h"
#include "Spark.h"
#include "ParticleSystem.h"

class Particles
{
	struct ParticleRef
	{
		int count;
		SPK::Ref<SPK::System> system;
	};

	std::map<std::string, ParticleRef> particles;

public:

	void Init();
	ParticleSystem* LoadParticle(const char* name, TaskExecutor::SingleTaskPool* taskPool, TaskExecutor::SingleTaskPool* renderPool, bool autoDelete);
	bool DecRef(SPK::Ref<SPK::System> system);
};
