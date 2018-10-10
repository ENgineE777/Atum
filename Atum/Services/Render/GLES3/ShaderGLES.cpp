
#include "ShaderGLES.h"
#include "DeviceGLES.h"
#include "VertexDeclGLES.h"
#include "TextureGLES.h"
#include "GeometryBufferGLES.h"
#include "Services/Core/Core.h"
#include "Services/File/FileInMemory.h"

ShaderGLES::ShaderGLES(Type tp, const char* name) : Shader(tp)
{
	int gl_type = GL_VERTEX_SHADER;
	if (tp == Pixel)
	{
		gl_type = GL_FRAGMENT_SHADER;
	}
	
	shader = glCreateShader(gl_type);

	FileInMemory buffer;
	char path[1024];
	StringUtils::Printf(path, 1024, "Shaders/GLES/%s", name);

	if (buffer.Load(path))
	{
		const char* buff = (const char*)buffer.GetData();
		glShaderSource(shader, 1, (const char**)&buff, NULL);

		glCompileShader(shader);

		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0)
		{
			GLchar *log = (GLchar *)malloc(logLength);
			glGetShaderInfoLog(shader, logLength, &logLength, log);

			core.Log("Render", "Shader %s with type %i was not compiled: %s", name, tp, log);

			free(log);
		}

		int status = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			glDeleteShader(shader);
		}
	}
}

ShaderGLES::Param& ShaderGLES::GetParam(const char* name)
{
	if (params.count(name) == 0)
	{
		params[name].param = glGetUniformLocation(program, name);
	}

	return params[name];
}

bool ShaderGLES::SetVector(const char* param, Vector4* v, int count)
{
	Param& p = GetParam(param);

	if (p.param != -1)
	{
		glUniform4fv(p.param, count, (float*)v);
		return true;
	}

	return false;
}

bool ShaderGLES::SetMatrix(const char* param, Matrix* m, int count)
{
	Param& p = GetParam(param);

	if (p.param != -1)
	{
		glUniformMatrix4fv(p.param, count, false, (float*)m);
		return true;
	}

	return false;
}

bool ShaderGLES::SetTexture(const char* param, Texture* tex)
{
	Param& p = GetParam(param);

	if (p.index == -1)
	{
		p.index = tex_index;
		tex_index++;
	}

	if (p.param != -1)
	{
		if (tex)
		{
			glActiveTexture(GL_TEXTURE0 + p.index);
			glBindTexture(GL_TEXTURE_2D, ((TextureGLES*)tex)->texture);
		}

		glUniform1i(p.param, p.index);

		return true;
	}

	return false;
}

void ShaderGLES::Apply()
{
}

void ShaderGLES::UpdateConstants()
{
}

void ShaderGLES::Release()
{

}