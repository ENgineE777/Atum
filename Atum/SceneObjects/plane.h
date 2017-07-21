
#include "Services/Render/Render.h"

class Plane : public Object
{
public:
	GeometryBuffer* buffer;
	Texture*        texture;

	struct VertexTri
	{
		Vector  position;
		Vector2 texCoord;
	};

	void Init(float height, const char* tex_name);
	void Render(float dt);
	void ShRender(float dt);
	void Render(Program* prg);
};
