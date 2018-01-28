#include "Quad.h"
#include "programs.h"

void Quad::Init()
{
	Programs::Init();

	int stride = sizeof(Vector2);
	buffer = render.GetDevice()->CreateBuffer(4, stride);

	Vector2* v = (Vector2*)buffer->Lock();

	v[0] = Vector2(0, 0);
	v[1] = Vector2(1, 0);
	v[2] = Vector2(0, 1);
	v[3] = Vector2(1, 1);

	buffer->Unlock();
}

void Quad::Draw(Texture* texture, Transform2D* trans)
{
	render.GetDevice()->SetVertexBuffer(0, buffer);

	Programs::quad_prg->Apply();

	Device::Viewport viewport;
	render.GetDevice()->GetViewport(viewport);

	Vector4 params[3];
	params[0] = Vector4(trans->offset.x, trans->offset.y, trans->size.x, trans->size.y);
	params[1] = Vector4(viewport.width, viewport.height, 0.5f, 0);
	params[2] = Vector4(1.0f, 1.0f, 1.0f, 1);

	Programs::quad_prg->VS_SetVector("desc", &params[0], 2);
	Programs::quad_prg->VS_SetMatrix("trans", &trans->local_trans, 1);
	Programs::quad_prg->PS_SetVector("color", &params[2], 1);
	Programs::quad_prg->PS_SetTexture("diffuseMap", texture);

	render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);
}