
#pragma once

#include "Services/Render/VertexDecl.h"

#ifdef PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

class VertexDeclGLES : public VertexDecl
{
public:

	struct VertexAttrib
	{
		int type;
		int components;
		int offset;
	};

	std::vector<VertexAttrib> attribs;

	VertexDeclGLES(int count, VertexDecl::ElemDesc* elems);
	void Apply();

	virtual void Release();
};
