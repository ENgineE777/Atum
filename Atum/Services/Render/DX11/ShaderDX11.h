
#pragma once

#include "Services/Render/Shader.h"
#include "Services/File/FileInMemory.h"
#include <vector>
#include <map>
#include <stdint.h>

/**
\ingroup gr_code_services_render_dx11
*/

class ShaderDX11 : public Shader
{
	friend class VertexDeclDX11;

	struct ID3D11VertexShader* vshader;
	struct ID3D11PixelShader*  pshader;

	struct ConstantBuffer
	{
		uint8_t* rawdata = nullptr;
		int      size = 0;
		int      slot = 0;
		bool     dirty = false;
		struct ID3D11Buffer* buffer = nullptr;
	};

	struct ShaderParamInfo
	{
		int slot = -1;
		int offset = 0;
		int size = 0;
	};

	std::vector<ConstantBuffer> buffers;
	std::map<std::string, ShaderParamInfo> constantsInfo;
	std::map<std::string, ShaderParamInfo> samplersInfo;

	FileInMemory buffer;
	class TextureDX11* textures[16];

public:

	ShaderDX11(Type tp, const char* name);

	virtual bool SetVector(const char* param, Vector4* v, int count);
	virtual bool SetMatrix(const char* param, Matrix* m, int count);
	virtual bool SetTexture(const char* param, Texture* tex);

	virtual void Apply();
	virtual void UpdateConstants();

	virtual void Release();
};