
#include "SimpleClient.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, SimpleClient, "SimpleClient")

META_DATA_DESC(SimpleClient)
BASE_SCENE_OBJ_PROP(SimpleClient)
META_DATA_DESC_END()

Matrix& SimpleClient::Trans()
{
	return transform;
}

bool SimpleClient::Is3DObject()
{
	return true;
}

void SimpleClient::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&SimpleClient::Work);
}

bool SimpleClient::Play()
{
	//client.listiner = &listiner;
	client.Connect("127.0.0.1", 6881);

	return true;
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