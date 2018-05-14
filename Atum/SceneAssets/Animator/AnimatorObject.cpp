
#include "AnimatorObject.h"

CLASSDECLDECL(AnimatorObject)

META_DATA_DESC(AnimatorObject)
STRING_PROP(AnimatorObject, animator_name, "", "Properties", "Animator")
BOOL_PROP(AnimatorObject, isLooped, false, "Properties", "Looped")
META_DATA_DESC_END()

AnimatorObject::AnimatorObject() : SceneObject()
{
}

AnimatorObject::~AnimatorObject()
{
}

void AnimatorObject::Init()
{
	Tasks()->AddTask(100, this, (Object::Delegate)&AnimatorObject::Work);
}

void AnimatorObject::Work(float dt)
{
	if (!owner->Playing() || !animator)
	{
		return;
	}

	curTime += dt;

	if (curTime > animator->timeLenght - 0.001f)
	{
		if (isLooped)
		{
			curTime = 0.0f;

			for (int i = 0; i<animator->players.size(); i++)
			{
				animator->players[i]->Reset();
			}
		}
		else
		{
			curTime = animator->timeLenght;
		}
	}

	for (int i = 0; i<animator->players.size(); i++)
	{
		animator->players[i]->SetTime(curTime);
	}
}

void AnimatorObject::Play()
{
	curTime = 0.0f;
	SceneObject::Play();
	animator = (Animator*)owner->FindAsset(animator_name.c_str());

	if (animator)
	{
		animator->Reset();
	}
}

void AnimatorObject::Stop()
{
	SceneObject::Stop();
	animator = nullptr;
}