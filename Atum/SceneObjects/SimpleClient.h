
#pragma once

#define WIN32_LEAN_AND_MEAN

#include "model.h"
#include "Services/Scene/SceneObject.h"
#include "Services/Network/NetworkService.h"

class SimpleClient : public SceneObject
{
public:

	class Listiner : public NetworkClient::Listiner
	{
	public:
		SimpleClient* owner = nullptr;
		virtual void OnDataRecievd(void* data, int size);
	};

	float koef = 0.0f;

	Listiner listiner;

	NetworkClient client;
	META_DATA_DECL(SimpleClient)

	CLASSDECLDIF(SceneObject, SimpleClient)
	SimpleClient();
	virtual ~SimpleClient();

	virtual void Init();
	virtual void Play();
	virtual void Work(float dt);
};
