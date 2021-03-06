#include "correction.h"
#include "programs.h"

CLASSREG(SceneObject, ColorCorrection, "ColorCorrection")

META_DATA_DESC(ColorCorrection)
BASE_SCENE_OBJ_PROP(ColorCorrection)
META_DATA_DESC_END()

void ColorCorrection::Init()
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float2, VertexDecl::Position, 0 } };
	vdecl = core.render.GetDevice()->CreateVertexDecl(1, desc);

	buffer = core.render.GetDevice()->CreateBuffer(4, sizeof(Vector2));

	Vector2* v = (Vector2*)buffer->Lock();

	v[0] = Vector2(-1,  1);
	v[1] = Vector2(1,  1);
	v[2] = Vector2(-1,-1);
	v[3] = Vector2(1,-1);

	buffer->Unlock();

	color_prg = core.render.GetProgram("ColorProgram");
	blur_prg = core.render.GetProgram("BlurProgram");
	combine_prg = core.render.GetProgram("CombineProgram");

	ring_rt[0] = nullptr;
	ring_rt[1] = nullptr;

	ReCreateRT();

	RenderTasks(false)->AddTask(ExecuteLevels::Prepare, this, (Object::Delegate)&ColorCorrection::SetRT);
	RenderTasks(false)->AddTask(ExecuteLevels::PostProcess, this, (Object::Delegate)&ColorCorrection::Draw);
}

void ColorCorrection::ReCreateRT()
{
	RELEASE(scene_rt)
	RELEASE(scene_depth)

	float scale = 0.5f;
#ifdef PLATFORM_ANDROID
	scale = 0.25f;
#endif

	for (int i = 0; i < 2; i++)
	{
		RELEASE(ring_rt[i])
		ring_rt[i] = core.render.GetDevice()->CreateTexture((int)(core.render.GetDevice()->GetWidth() * scale), (int)(core.render.GetDevice()->GetHeight() * scale), Texture::FMT_A8R8G8B8, 1, true, Texture::Tex2D);
		ring_rt[i]->SetAdressU(Texture::Clamp);
		ring_rt[i]->SetAdressV(Texture::Clamp);
	}

	scene_rt = core.render.GetDevice()->CreateTexture(core.render.GetDevice()->GetWidth(), core.render.GetDevice()->GetHeight(), Texture::FMT_A8R8G8B8, 1, true, Texture::Tex2D);
	scene_depth = core.render.GetDevice()->CreateTexture(core.render.GetDevice()->GetWidth(), core.render.GetDevice()->GetHeight(), Texture::FMT_D16, 1, true, Texture::Tex2D);
}

float ColorCorrection::ComputeGaussian(float n)
{
	float theta = 4.0f;

	return (float)((1.0 / sqrtf(2 * Math::PI * theta)) * expf(-(n * n) / (2 * theta * theta)));
}

void ColorCorrection::SetRT(float dt)
{
	core.render.GetDevice()->SetRenderTarget(0, scene_rt);
	core.render.GetDevice()->SetDepth(scene_depth);

	core.render.GetDevice()->Clear(true, COLOR_BLACK, true, 1.0f);
}

void ColorCorrection::Draw(float dt)
{
	core.render.GetDevice()->RestoreRenderTarget();

	core.render.GetDevice()->SetRenderTarget(0, ring_rt[0]);
	core.render.GetDevice()->SetDepth(NULL);

	core.render.GetDevice()->SetVertexDecl(vdecl);
	core.render.GetDevice()->SetVertexBuffer(0, buffer);

	Vector4 threshold;
	threshold.x = 0.6f;

	core.render.GetDevice()->SetProgram(color_prg);

	color_prg->SetVector(Shader::Type::Vertex, "treshold", &threshold, 1);
	color_prg->SetTexture(Shader::Type::Pixel, "rt", scene_rt);

	core.render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);

	core.render.GetDevice()->SetRenderTarget(0, ring_rt[1]);

	core.render.GetDevice()->SetProgram(blur_prg);
	blur_prg->SetTexture(Shader::Type::Pixel, "rt", ring_rt[0]);

	Vector4 samples[15];

	samples[0].z = ComputeGaussian(0);
	samples[0].x = 0;
	samples[0].y = 0;

	float totalWeights = samples[0].z;

	for (int i = 0; i < 7; i++)
	{
		float weight = ComputeGaussian((float)i + 1.0f);

		samples[i * 2 + 1].z = weight;
		samples[i * 2 + 2].z = weight;

		totalWeights += weight * 2;

		float sampleOffset = i * 2 + 1.5f;

		Vector2 delta = Vector2(1.0f / (float)ring_rt[0]->GetWidth(), 1.0f / (float)ring_rt[0]->GetHeight()) * sampleOffset;

		samples[i * 2 + 1].x = delta.x;
		samples[i * 2 + 1].y = 0.0f;
		samples[i * 2 + 1].w = delta.y;

		samples[i * 2 + 2].x =-delta.x;
		samples[i * 2 + 2].y = 0.0f;
		samples[i * 2 + 2].w =-delta.y;
	}

	for (int i = 0; i < 15; i++)
	{
		samples[i].z /= totalWeights;
	}

	blur_prg->SetVector(Shader::Type::Pixel, "samples", samples, 15);

	core.render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);

	core.render.GetDevice()->SetRenderTarget(0, ring_rt[0]);

	for (int i = 0; i < 15; i++)
	{
		samples[i].x = 0;
		samples[i].y = samples[i].w;
	}

	blur_prg->SetTexture(Shader::Type::Pixel, "rt", ring_rt[1]);
	blur_prg->SetVector(Shader::Type::Pixel, "samples", samples, 15);

	core.render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);

	core.render.GetDevice()->RestoreRenderTarget();

	core.render.GetDevice()->SetProgram(combine_prg);
	combine_prg->SetTexture(Shader::Type::Pixel, "rt", ring_rt[0]);
	combine_prg->SetTexture(Shader::Type::Pixel, "rt1", scene_rt);

	core.render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);
}

void ColorCorrection::Release()
{
	RELEASE(scene_rt)
	RELEASE(scene_depth)
	SceneObject::Release();
}