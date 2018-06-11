
#include "SimpleClient.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, SimpleClient)

META_DATA_DESC(SimpleClient)
META_DATA_DESC_END()

SimpleClient::SimpleClient() : SceneObject()
{
}

SimpleClient::~SimpleClient()
{
}

void SimpleClient::Init()
{
	Tasks()->AddTask(100, this, (Object::Delegate)&SimpleClient::Work);
}

void SimpleClient::Play()
{
	//client.listiner = &listiner;
	client.Connect("127.0.0.1", 6881);
}

void SimpleClient::Work(float dt)
{
	if (Playing())
	{
		client.Update();
	}

	koef -= dt * 3.0f;

	if (koef < 0.0f)
	{
		koef = 0.0f;
	}

	Color color = COLOR_RED;

	color.g = koef;
	color.b = koef;

	render.DebugSphere(transform.Pos(), color, 0.5f);
}