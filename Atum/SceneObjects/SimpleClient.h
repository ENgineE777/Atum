
#pragma once

#define WIN32_LEAN_AND_MEAN

#include "model.h"
#include "Services/Scene/SceneObject.h"
#include "Services/Network/Network.h"

class SimpleClient : public SceneObject
{
public:

	float koef = 0.0f;

	NetworkClient client;
	META_DATA_DECL(SimpleClient)

	CLASSDECLDIF(SceneObject, SimpleClient)
	SimpleClient();
	virtual ~SimpleClient();

	virtual void Init();
	virtual void Play();
	virtual void Work(float dt);
};
