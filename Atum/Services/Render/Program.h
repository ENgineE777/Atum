
#pragma once

#include "Shader.h"
#include "Texture.h"
#include <map>
#include "Support/ClassFactory.h"

class Program
{
protected:
	Shader* vshader = nullptr;
	Shader* pshader = nullptr;

	friend class DeviceDX11;
	friend class DeviceGLES;

	static std::map<std::string, Vector4>  vectors;
	static std::map<std::string, Matrix>   matrixes;
	static std::map<std::string, Texture*> textures;

public:

	enum Stage
	{
		Vertex,
		Pixel
	};

#ifdef PLATFORM_ANDROID
	int program = -1;
#endif

	static void SetVector(const char* param, Vector4* v);
	static void SetMatrix(const char* param, Matrix* mat);
	static void SetTexture(const char* param, Texture* texture);

	virtual bool Init();

	virtual const char* GetVsName() = 0;
	virtual const char* GetPsName() = 0;

	virtual void ApplyStates() {};

	void SetVector(Stage stage, const char* param, Vector4* v, int count);
	void SetMatrix(Stage stage, const char* param, Matrix* mat, int count);
	void SetTexture(Stage stage, const char* param, Texture* texture);
};

CLASSFACTORYDEF(Program)