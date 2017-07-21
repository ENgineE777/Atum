
#include "Render.h"
#include "Program.h"

Program* Program::current = NULL;
int Program::set_stage = 0;

std::map<std::string, Vector4>  Program::vectors;
std::map<std::string, Matrix>   Program::matrixes;
std::map<std::string, Texture*> Program::textures;


void Program::SetVector(const char* param, Vector4* v)
{
	Vector4& vec = vectors[param];
	memcpy(&vec.x, &v->x, 4 * 4);
}

void Program::SetMatrix(const char* param, Matrix* mat)
{
	Matrix& mt = matrixes[param];
	memcpy(&mt.m[0][0], &mat->m[0][0], 16 * 4);
}

void Program::SetTexture(const char* param, Texture* texture)
{
	textures[param] = texture;
}

Program::Program()
{	
	vshader = NULL;
	pshader = NULL;
}
	
bool Program::Init()
{
	if (GetVsName())
	{
		vshader = render.GetDevice()->CreateShader(Shader::Vertex, GetVsName());
	}

	if (GetPsName())
	{
		pshader = render.GetDevice()->CreateShader(Shader::Pixel, GetPsName());
	}

	return true;
}

void Program::Apply()
{
	if (current != this)
	{
		set_stage = 0;
		current = this;
	}
}

void Program::SetShaders()
{
	if (set_stage == 0)
	{
		if (vshader) vshader->Apply();
		if (pshader) pshader->Apply();

		set_stage = 1;
	}

	if (vshader) vshader->UpdateConstants();
	if (pshader) pshader->UpdateConstants();
}

void Program::VS_SetVector(const char* param, Vector4* v, int count)
{
	if (vshader) vshader->SetVector(param, v, count);
}

void Program::VS_SetMatrix(const char* param, Matrix* mat, int count)
{
	if (vshader) vshader->SetMatrix(param, mat, count);
}

void Program::VS_SetTexture(const char* param, Texture* texture)
{
	if (vshader) vshader->SetTexture(param, texture);
}

void Program::PS_SetVector(const char* param, Vector4* v, int count)
{
	if (pshader) pshader->SetVector(param, v, count);
}

void Program::PS_SetMatrix(const char* param, Matrix* mat, int count)
{
	if (pshader) pshader->SetMatrix(param, mat, count);
}

void Program::PS_SetTexture(const char* param, Texture* texture)
{
	if (pshader) pshader->SetTexture(param, texture);
}
