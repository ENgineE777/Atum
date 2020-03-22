
#pragma once

#include "Services/Render/Render.h"

class DebugSpheres : public Object
{
	enum
	{
		RigsCount = 16,
		SidesCount = 16,
		PrimCount = RigsCount * SidesCount * 2
	};

	struct Sphere
	{
		Vector3 pos;
		Color   color;
		float   radius;
	};

	struct Vertex
	{
		Vector3  pos;
		Vector3  normal;
		uint32_t color;
	};

	std::vector<Sphere> spheres;

	Program* prg;
	VertexDecl* vdecl;
	DataBuffer* vbuffer;
	DataBuffer* ibuffer;

public:
	
	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddSphere(Vector3 pos, Color color, float radius);
	void Draw(float dt);
	void Release();
};
