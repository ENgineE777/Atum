
#pragma once

#include "Support/Support.h"

/**
\ingroup gr_code_services_render
*/

class VertexDecl
{
public:

	enum ElemType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		UInt,
		UInt2,
		UInt3,
		UInt4,
		Ubyte4
	};

	enum ElemSemantic
	{
		Position,
		Texcoord,
		Color
	};

	struct ElemDesc
	{
		ElemType     type;
		ElemSemantic semantic;
		int          index;
	};

	virtual void Release() = 0;
};
