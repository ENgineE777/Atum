
#include <string.h>
#include <Ws2tcpip.h>

#include "SimpleServer.h"
#include "Services/Render/Render.h"

CLASSDECLDECL(SceneObject, SimpleServer)

META_DATA_DESC(SimpleServer)
META_DATA_DESC_END()

SimpleServer::SimpleServer() : SceneObject()
{
}

SimpleServer::~SimpleServer()
{
}

void SimpleServer::Init()
{
	Tasks()->AddTask(100, this, (Object::Delegate)&SimpleServer::Work);
	Tasks()->AddTask(100, this, (Object::Delegate)&SimpleServer::Blink, 1.0f);
}


void SimpleServer::Play()
{
	server.Start(NULL, 6881);
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

	render.DebugSphere(transform.Pos(), color, 0.5f);
}