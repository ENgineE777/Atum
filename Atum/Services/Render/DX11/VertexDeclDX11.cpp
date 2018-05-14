
#include "VertexDeclDX11.h"
#include "DeviceDX11.h"

VertexDeclDX11::VertexDeclDX11(int count, VertexDecl::ElemDesc* elems)
{
	int offset = 0;

	DXGI_FORMAT formats[] = { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT,
	                          DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT,
	                          DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT,
	                          DXGI_FORMAT_R8G8B8A8_UNORM };

	uint8_t offsets[] = { 4, 8, 12, 16, 4, 8, 12, 16, 4, 8, 12, 16, 4 };

	layoutDesc.resize(count);

	for (int i = 0; i < count; i++)
	{
		D3D11_INPUT_ELEMENT_DESC& elementDesc = layoutDesc[i];

		switch (elems[i].semantic)
		{
			case Position:
				elementDesc.SemanticName = "POSITION";
				break;
			case Texcoord:
				elementDesc.SemanticName = "TEXCOORD";
				break;
			case Color:
				elementDesc.SemanticName = "COLOR";
				break;
		}

		elementDesc.SemanticIndex = elems[i].index;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = offset;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		int index = elems[i].type;
		elementDesc.Format = formats[index];
		offset += offsets[index];
	}
}

void VertexDeclDX11::Apply(ShaderDX11* shader)
{
	ID3D11InputLayout* layout = nullptr;

	if (layouts.count(shader) == 0)
	{
		DeviceDX11::instance->pd3dDevice->CreateInputLayout(&layoutDesc[0], (UINT)layoutDesc.size(), shader->buffer.GetData(), (size_t)shader->buffer.GetSize(), &layout);

		layouts[shader] = layout;
	}
	else
	{
		layout = layouts[shader];
	}

	if (layout)
	{
		DeviceDX11::instance->immediateContext->IASetInputLayout(layout);
	}
}

void VertexDeclDX11::Release()
{
	typedef std::map<ShaderDX11*, ID3D11InputLayout*>::iterator it_type;

	for (it_type iterator = layouts.begin(); iterator != layouts.end(); iterator++)
	{
		RELEASE(iterator->second);
	}

	delete this;
}