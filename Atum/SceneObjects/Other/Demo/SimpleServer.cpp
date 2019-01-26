
#include <string.h>

#include "SimpleServer.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, SimpleServer, "SimpleServer")

META_DATA_DESC(SimpleServer)
BASE_SCENE_OBJ_PROP(SimpleServer)
META_DATA_DESC_END()

void SimpleServer::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&SimpleServer::Work);
	Tasks(false)->AddTask(100, this, (Object::Delegate)&SimpleServer::Blink, 1.0f);
}

bool SimpleServer::Play()
{
	server.Start(NULL, 6881);

	return true;
}

void SimpleServer::Blink(float dt)
{
	if (!Playing())
	{
		return;
	}

	koef = 1.0f;

	server.Send2All(&koef, 4);
}

Matrix& SimpleServer::Trans()
{
	return transform;
}

bool SimpleServer::Is3DObject()
{
	return true;
}

void SimpleServer::Work(float dt)
{
	if (Playing())
	{
		server.Update();
	}

	koef -= dt * 3.0f;

	if (koef < 0.0f)
	{
		koef = 0.0f;
	}

	Color color = COLOR_RED;

	color.g = koef;
	color.b = koef;

	core.render.DebugSphere(transform.Pos(), color, 0.5f);
}