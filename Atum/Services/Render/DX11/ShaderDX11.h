
#pragma once

#include "Services/Render/Shader.h"
#include <vector>
#include <map>
#include <stdint.h>

class ShaderDX11 : public Shader
{
	struct ID3D11VertexShader* vshader;
	struct ID3D11PixelShader* pshader;
	struct ID3D11InputLayout*  layout;

	struct ConstantBuffer
	{
		uint8_t*	  rawdata;
		int           size;
		int			  slot;
		struct ID3D11Buffer* buffer;
		bool		  dirty;

		ConstantBuffer()
		{
			rawdata = NULL;
			slot = 0;
			size = 0;
			buffer = NULL;
			dirty = false;
		};
	};

	struct ShaderParamInfo
	{
		int slot;
		int offset;
		int size;

		ShaderParamInfo()
		{
			slot = -1;
			offset = 0;
			size = 0;
		};
	};

	std::vector<ConstantBuffer> buffers;
	std::map<std::string, ShaderParamInfo> constantsInfo;
	std::map<std::string, ShaderParamInfo> samplersInfo;

	Texture* textures[16];
	bool texDirty;

public:

	ShaderDX11(Type tp, const char* name);

	virtual bool SetVector(const char* param, Vector4* v, int count);
	virtual bool SetMatrix(const char* param, Matrix* m, int count);
	virtual bool SetTexture(const char* param, Texture* tex);

	virtual void Apply();
	virtual void UpdateConstants();

	virtual void Release();
};