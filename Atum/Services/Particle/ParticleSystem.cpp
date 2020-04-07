
#include "Services/Core/Core.h"
#include "Services/Scene/ExecuteLevels.h"

void ParticleSystem::Init(SPK::Ref<SPK::System> set_res, TaskExecutor::SingleTaskPool* set_taskPool, TaskExecutor::SingleTaskPool* set_renderPool, bool set_autoDelete)
{
	res = set_res;
	taskPool = set_taskPool;
	taskPool->AddTask(0, this, (Object::Delegate)&ParticleSystem::Update);

	renderPool = set_renderPool;
	renderPool->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&ParticleSystem::Render);

	autoDelete = set_autoDelete;

	system = SPK::SPKObject::copy(set_res);
	system->initialize();
}

void ParticleSystem::SetTransform(Matrix& transform)
{
	system->getTransform().set(transform.matrix);
	system->updateTransform();
}

void ParticleSystem::SetVisible(bool set)
{
	visible = set;
}

bool ParticleSystem::IsVisible()
{
	return visible;
}

void ParticleSystem::Play(bool set)
{
	playing = set;
}

bool ParticleSystem::IsPlaying()
{
	return playing;
}

void ParticleSystem::SetAutoDelete(bool set)
{
	autoDelete = set;
}

bool ParticleSystem::IfAutoDelete()
{
	return autoDelete;
}

bool ParticleSystem::IsSysyemActive()
{
	return system->isActive();
}

void ParticleSystem::Restart()
{
	float mat[16];
	memcpy(mat, system->getTransform().getLocal(), 16 * 4);
	system = SPK::SPKObject::copy(res);
	system->initialize();

	system->getTransform().set(mat);
	system->updateTransform();
}

void ParticleSystem::Update(float dt)
{
	if (playing && visible)
	{
		system->updateParticles(dt);
	}

	if (autoDelete && !system->isActive())
	{
		Release();
	}
}

void ParticleSystem::Render(float dt)
{
	if (visible)
	{
		system->renderParticles();
	}
}

void ParticleSystem::Release()
{
	taskPool->DelAllTasks(this);
	renderPool->DelAllTasks(this);

	core.particles.DecRef(res);

	delete this;
}