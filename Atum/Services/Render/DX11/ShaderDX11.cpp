
#include "ShaderDX11.h"
#include "DeviceDX11.h"
#include "VertexDeclDX11.h"
#include "TextureDX11.h"

#include "d3d11.h"
#include "d3dcompiler.h"

ShaderDX11::ShaderDX11(Type tp, const char* name) : Shader(tp)
{
	char path[1024];
	StringUtils::Printf(path, 1024, "Shaders/PC/%s", name);
	if (!buffer.Load(path))
	{
		return;
	}

	if (tp == Shader::Vertex)
	{
		DeviceDX11::instance->pd3dDevice->CreateVertexShader(buffer.GetData(), buffer.GetSize(), NULL, &vshader);
	}
	else
	{
		DeviceDX11::instance->pd3dDevice->CreatePixelShader(buffer.GetData(), buffer.GetSize(), NULL, &pshader);
	}

	ID3D11ShaderReflection* pVertexShaderReflection;
	D3DReflect(buffer.GetData(), buffer.GetSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection);

	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	for (unsigned int i = 0; i < shaderDesc.ConstantBuffers; i++)
	{
		D3D11_SHADER_BUFFER_DESC shaderBuffer;
		ID3D11ShaderReflectionConstantBuffer* pConstBuffer = pVertexShaderReflection->GetConstantBufferByIndex(i);

		pConstBuffer->GetDesc(&shaderBuffer);

		D3D11_SHADER_INPUT_BIND_DESC bindingDesc;
		pVertexShaderReflection->GetResourceBindingDescByName(shaderBuffer.Name, &bindingDesc);

		buffers.push_back(ConstantBuffer());

		ConstantBuffer& buffer = buffers.back();
		buffer.size = shaderBuffer.Size;
		buffer.rawdata = (byte*)malloc(buffer.size);

		buffer.slot = bindingDesc.BindPoint;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = buffer.size;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		DeviceDX11::instance->pd3dDevice->CreateBuffer(&bd, NULL, &buffer.buffer);

		for (unsigned int j = 0; j < shaderBuffer.Variables; j++)
		{
			ID3D11ShaderReflectionVariable * pVariable = pConstBuffer->GetVariableByIndex(j);

			D3D11_SHADER_VARIABLE_DESC varDesc;
			pVariable->GetDesc(&varDesc);

			D3D11_SHADER_TYPE_DESC varType;
			ID3D11ShaderReflectionType * pType = pVariable->GetType();

			pType->GetDesc(&varType);

			ShaderParamInfo& param = constantsInfo[varDesc.Name];

			param.slot = i;
			param.offset = varDesc.StartOffset;
			param.size = varDesc.Size;
		}
	}

	for (unsigned int i = 0; i<shaderDesc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		pVertexShaderReflection->GetResourceBindingDesc(i, &bindDesc);

		if (bindDesc.Type == D3D_SIT_TEXTURE)
		{
			ShaderParamInfo& param = samplersInfo[bindDesc.Name];

			param.slot = bindDesc.BindPoint;
		}
	}

	for (int i = 0; i < 16; i++)
	{
		textures[i] = nullptr;
	}
}

bool ShaderDX11::SetVector(const char* param, Vector4* v, int count)
{
	ShaderParamInfo& spInfo = constantsInfo[param];

	if (spInfo.slot == -1)
	{
		return false;
	}

	ConstantBuffer* buffer = &buffers[spInfo.slot];

	int sz = min(spInfo.size, sizeof(float) * 4 * count);
	memcpy((void*)&buffer->rawdata[spInfo.offset], &v->x, sz);

	buffer->dirty = true;

	return true;
}

bool ShaderDX11::SetMatrix(const char* param, Matrix* m, int count)
{
	ShaderParamInfo& spInfo = constantsInfo[param];

	if (spInfo.slot == -1)
	{
		return false;
	}

	ConstantBuffer* buffer = &buffers[spInfo.slot];

	Matrix tmp(false);
	memcpy(&tmp.matrix[0], &m->matrix[0], 64);
	
	tmp.Transpose();

	memcpy((void*)&buffer->rawdata[spInfo.offset], &tmp.matrix[0], spInfo.size);

	buffer->dirty = true;

	return true;
}

bool ShaderDX11::SetTexture(const char* param, Texture* tex)
{
	ShaderParamInfo& spInfo = samplersInfo[param];

	if (spInfo.slot == -1)
	{
		return false;
	}

	textures[spInfo.slot] = (TextureDX11*)tex;

	return true;
}

void ShaderDX11::Apply()
{
	if (sdr_type == Shader::Vertex)
	{
		DeviceDX11::instance->immediateContext->VSSetShader(vshader, 0, 0);
	}
	else
	{
		DeviceDX11::instance->immediateContext->PSSetShader(pshader, 0, 0);
	}
}

void ShaderDX11::UpdateConstants()
{
	for (int i = 0; i<buffers.size(); i++)
	{
		ConstantBuffer* buffer = &buffers[i];

		if (buffer->dirty)
		{
			float k = *((float*)buffer->rawdata);
			DeviceDX11::instance->immediateContext->UpdateSubresource(buffer->buffer, 0, NULL, buffer->rawdata, 0, 0);

			buffer->dirty = false;
		}

		if (sdr_type == Pixel)
		{
			DeviceDX11::instance->immediateContext->PSSetConstantBuffers(i, 1, &buffer->buffer);
		}
		else
		{
			DeviceDX11::instance->immediateContext->VSSetConstantBuffers(i, 1, &buffer->buffer);
		}
	}

	typedef std::map<std::string, ShaderParamInfo>::iterator it_type;

	for (it_type iterator = samplersInfo.begin(); iterator != samplersInfo.end(); iterator++)
	{
		ShaderParamInfo& spInfo = iterator->second;

		if (spInfo.slot == -1)
		{
			continue;
		}

		if (textures[spInfo.slot])
		{
			ID3D11ShaderResourceView* sh_view = (ID3D11ShaderResourceView*)textures[spInfo.slot]->srview;

			textures[spInfo.slot]->Apply(spInfo.slot);
			DeviceDX11::instance->immediateContext->PSSetShaderResources(spInfo.slot, 1, &sh_view);
		}
	}
}

void ShaderDX11::Release()
{

}