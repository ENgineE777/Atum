
#include "Render.h"
#include "Program.h"

CLASSFACTORYDECL(Program)

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

void Program::SetVector(Stage stage, const char* param, Vector4* v, int count)
{
	if (stage == Program::Vertex)
	{
		if (vshader) vshader->SetVector(param, v, count);
	}
	if (stage == Program::Pixel)
	{
		if (pshader) pshader->SetVector(param, v, count);
	}
}

void Program::SetMatrix(Stage stage, const char* param, Matrix* mat, int count)
{
	if (stage == Program::Vertex)
	{
		if (vshader) vshader->SetMatrix(param, mat, count);
	}
	if (stage == Program::Pixel)
	{
		if (pshader) pshader->SetMatrix(param, mat, count);
	}
}

void Program::SetTexture(Stage stage, const char* param, Texture* texture)
{
	if (stage == Program::Vertex)
	{
		if (vshader) vshader->SetTexture(param, texture);
	}
	if (stage == Program::Pixel)
	{
		if (pshader) pshader->SetTexture(param, texture);
	}
}

