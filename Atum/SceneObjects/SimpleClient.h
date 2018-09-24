
#pragma once

#define WIN32_LEAN_AND_MEAN

#include "model.h"
#include "Services/Scene/SceneObject.h"
#include "Services/Network/Network.h"

class SimpleClient : public SceneObject
{
public:

	Matrix transform;
	float koef = 0.0f;

	NetworkClient client;
	META_DATA_DECL(SimpleClient)

	virtual ~SimpleClient() = default;

	Matrix& Trans() override;
	bool Is3DObject() override;

	void Init() override;
	bool Play() override;
	void Work(float dt);
};
