
#pragma once

#include "Services/Render/VertexDecl.h"
#include "d3d11.h"
#include "ShaderDX11.h"

class VertexDeclDX11 : public VertexDecl
{
public:

	std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;

	std::map<ShaderDX11*, ID3D11InputLayout*> layouts;

	VertexDeclDX11(int count, VertexDecl::ElemDesc* elems);
	void Apply(ShaderDX11* shader);

	virtual void Release();
};
