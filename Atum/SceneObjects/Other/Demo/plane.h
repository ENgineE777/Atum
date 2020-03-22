
#include "Services/Render/Render.h"

class Plane : public Object
{
public:
	VertexDecl* vdecl;
	DataBuffer* buffer;
	Texture* texture;

	struct VertexTri
	{
		Vector3 position;
		Vector2 texCoord;
	};

	void Init(float height, const char* tex_name);
	void Render(float dt);
	void ShRender(float dt);
	void Render(Program* prg);
};
