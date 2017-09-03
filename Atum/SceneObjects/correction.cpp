#include "correction.h"
#include "programs.h"

void ColorCorrection::Init()
{
	buffer = render.GetDevice()->CreateBuffer(4, sizeof(Vector2));

	Vector2* v = (Vector2*)buffer->Lock();

	v[0] = Vector2(-1,  1);
	v[1] = Vector2(1,  1);
	v[2] = Vector2(-1,-1);
	v[3] = Vector2(1,-1);

	buffer->Unlock();

	for (int i = 0; i < 2; i++)
	{
		ring_rt[i] = render.GetDevice()->CreateTexture(400, 300, Texture::FMT_A8R8G8B8, 1, true, Texture::Tex2D);
		ring_rt[i]->SetAdressU(Texture::Border);
		ring_rt[i]->SetAdressV(Texture::Border);
	}
}

float ComputeGaussian(float n)
{
	float theta = 4.0f;

	return (float)((1.0 / sqrtf(2 * PI * theta)) * expf(-(n * n) / (2 * theta * theta)));
}


void ColorCorrection::Draw(Texture* rt)
{
	render.GetDevice()->SetRenderTarget(0, ring_rt[0]);
	render.GetDevice()->SetDepth(NULL);

	render.GetDevice()->SetVertexBuffer(0, buffer);

	Vector4 threshold;
	threshold.x = 0.6f;

	Programs::color_prg->Apply();
	Programs::color_prg->PS_SetVector("treshold", &threshold, 1);
	Programs::color_prg->PS_SetTexture("rt", rt);

	render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);



	render.GetDevice()->SetRenderTarget(0, ring_rt[1]);

	Programs::blur_prg->Apply();
	Programs::blur_prg->PS_SetTexture("rt", ring_rt[0]);

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

	Programs::blur_prg->PS_SetVector("samples", samples, 15);

	render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);

	render.GetDevice()->SetRenderTarget(0, ring_rt[0]);

	for (int i = 0; i < 15; i++)
	{
		samples[i].x = 0;
		samples[i].y = samples[i].w;
	}

	Programs::blur_prg->PS_SetTexture("rt", ring_rt[1]);
	Programs::blur_prg->PS_SetVector("samples", samples, 15);


	render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);

	render.GetDevice()->RestoreRenderTarget();

	threshold.x = 0.0f;

	Programs::combine_prg->Apply();
	Programs::combine_prg->PS_SetTexture("rt", ring_rt[0]);
	Programs::combine_prg->PS_SetTexture("rt1", rt);

	render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);
}