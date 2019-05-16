
#pragma once

#include "Support/Support.h"
#include "Services/Render/Texture.h"

/**
\ingroup gr_code_services_render
*/

/**
\brief PhysScene

This is representation of a physical scene. Objecte to a scene are adding via proper
methods of PhysScene.

*/

class Shader
{
public:
	enum Type
	{
		Vertex = 0 /*!< Hardware button in pressed state */,
		Pixel  = 1 /*!< Hardware button in pressed state */
	};

	Shader(Type tp) { sdr_type = tp; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual bool SetVector(const char* param, Vector4* v, int count) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual bool SetMatrix(const char* param, Matrix* m, int count) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual bool SetTexture(const char* param, Texture* tex) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void UpdateConstants() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Apply() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Release() = 0;

protected:
	Type sdr_type;
};