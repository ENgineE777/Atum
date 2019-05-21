
#pragma once

#include "Support/Support.h"
#include "Services/Render/Texture.h"

/**
\ingroup gr_code_services_render
*/

/**
\brief Shader

Class that holds ready for use shader and allows set up constants.

*/

class Shader
{
public:
	enum Type
	{
		Vertex = 0 /*!< Vertex shader */,
		Pixel  = 1 /*!< Pixel shader */
	};

#ifndef DOXYGEN_SKIP

	Shader(Type tp) { sdr_type = tp; };

	virtual void UpdateConstants() = 0;
	virtual void Apply() = 0;
#endif

	/**
	\brief Set 4 component vector for a parameter

	\param[in] param Name of a parameter
	\param[in] v Pointer to vectors
	\param[in] count Number of vectors needed to be set

	\return Result of an operation
	*/
	virtual bool SetVector(const char* param, Vector4* v, int count) = 0;

	/**
	\brief Set matrices for a parameter

	\param[in] param Name of a parameter
	\param[in] m Pointer to a matrices
	\param[in] count Number of matrices needed to be set

	\return Result of an operation
	*/
	virtual bool SetMatrix(const char* param, Matrix* m, int count) = 0;

	/**
	\brief Set texture for a parameter

	\param[in] param Name of a parameter
	\param[in] tex Pointer to a texture

	\return Result of an operation
	*/
	virtual bool SetTexture(const char* param, Texture* tex) = 0;

	/**
	\brief Shader should released only via this method
	*/
	virtual void Release() = 0;

protected:
	Type sdr_type;
};