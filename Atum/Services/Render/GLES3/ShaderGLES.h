
#pragma once

#include "Services/Render/Shader.h"
#include <vector>
#include <map>
#include <stdint.h>

#ifdef PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

#ifdef PLATFORM_IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

class ShaderGLES : public Shader
{
	Texture* textures[16];

	struct Param
	{
		GLuint param = -1;
		int    index = -1;
	};

	int tex_index = 0;

	std::map<std::string, Param> params;

	Param& GetParam(const char* name);

public:

	GLuint shader = -1;
	GLuint program;

	ShaderGLES(Type tp, const char* name);

	virtual bool SetVector(const char* param, Vector4* v, int count);
	virtual bool SetMatrix(const char* param, Matrix* m, int count);
	virtual bool SetTexture(const char* param, Texture* tex);

	virtual void Apply();
	virtual void UpdateConstants();

	virtual void Release();
};
