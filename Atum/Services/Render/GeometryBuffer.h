
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

class GeometryBuffer
{
protected:

	int size;
	int stride;

public:

	GeometryBuffer(int count, int strd) { size = count * strd; stride = strd; };
	virtual ~GeometryBuffer() {};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void* Lock() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Unlock() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	int GetSize() { return size; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	int GetStride() { return stride; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Release() = 0;
};
