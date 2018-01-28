
#include "Services/Render/Render.h"

class Quad
{
public:

	GeometryBuffer* buffer;

	void Init();
	void Draw(Texture* texture, Transform2D* trans);
};
