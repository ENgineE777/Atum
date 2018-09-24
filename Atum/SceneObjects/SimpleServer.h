
#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Services/Scene/SceneObject.h"
#include "Services/Network/Network.h"

class SimpleServer : public SceneObject
{
public:

	float koef = 0.0f;
	Matrix transform;

	META_DATA_DECL(SimpleServer)

	NetworkServer server;
	virtual ~SimpleServer() = default;

	Matrix& Trans() override;
	bool Is3DObject() override;

	void Init() override;
	bool Play() override;
	void Work(float dt);
	void Blink(float dt);
};
