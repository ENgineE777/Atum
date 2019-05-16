
#pragma once

#include "Support/Support.h"

/**
\ingroup gr_code_services_render
*/

/**
\brief PhysScene

This is representation of a physical scene. Objecte to a scene are adding via proper
methods of PhysScene.

*/

class VertexDecl
{
public:

	enum ElemType
	{
		Float /*!< Hardware button in pressed state */,
		Float2 /*!< Hardware button in pressed state */,
		Float3 /*!< Hardware button in pressed state */,
		Float4 /*!< Hardware button in pressed state */,
		Int /*!< Hardware button in pressed state */,
		Int2 /*!< Hardware button in pressed state */,
		Int3 /*!< Hardware button in pressed state */,
		Int4 /*!< Hardware button in pressed state */,
		UInt /*!< Hardware button in pressed state */,
		UInt2 /*!< Hardware button in pressed state */,
		UInt3 /*!< Hardware button in pressed state */,
		UInt4 /*!< Hardware button in pressed state */,
		Ubyte4 /*!< Hardware button in pressed state */
	};

	enum ElemSemantic
	{
		Position /*!< Hardware button in pressed state */,
		Texcoord /*!< Hardware button in pressed state */,
		Color /*!< Hardware button in pressed state */
	};

	struct ElemDesc
	{
		ElemType     type;
		ElemSemantic semantic;
		int          index;
	};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Release() = 0;
};
