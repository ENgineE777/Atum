
#pragma once

#include "Shader.h"
#include "Texture.h"
#include <map>
#include "Support/ClassFactory.h"

/**
\ingroup gr_code_services_render
*/

/**
\brief Program

Combination of vertex and pixel shaders into program with set up of render states.

*/

class Program
{
protected:
	Shader* vshader = nullptr;
	Shader* pshader = nullptr;

	friend class DeviceDX11;
	friend class DeviceGLES;

	static std::map<std::string, Vector4>  vectors;
	static std::map<std::string, Matrix>   matrixes;
	static std::map<std::string, Texture*> textures;

public:

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
	int program = -1;
#endif

#ifndef DOXYGEN_SKIP
	virtual bool Init();
#endif

	/**
	\brief Store a 4 componet vector in global pool

	\param[in] param Name of a parameter
	\param[in] v Pointer to a vector
	*/
	static void SetVector(const char* param, Vector4* v);

	/**
	\brief Store a matrix in global pool

	\param[in] param Name of a parameter
	\param[in] mat Pointer to a matrix
	*/
	static void SetMatrix(const char* param, Matrix* mat);

	/**
	\brief Store texture in global pool

	\param[in] param Name of a parameter
	\param[in] texture Pointer to a texture
	*/
	static void SetTexture(const char* param, Texture* texture);

	/**
	\brief Return name of vertex shader

	\return Name of vertex shader
	*/
	virtual const char* GetVsName() = 0;

	/**
	\brief Return name of pixel shader

	\return Name of pixel shader
	*/
	virtual const char* GetPsName() = 0;

	/**
	\brief Apply render states before render
	*/
	virtual void ApplyStates() {};

	/**
	\brief Set 4 component vector for a parameter

	\param[in] shader_type Type of shader
	\param[in] param Name of a parameter
	\param[in] v Pointer to vectors
	\param[in] count Number of vectors needed to be set

	\return Result of an operation
	*/
	void SetVector(Shader::Type shader_type, const char* param, Vector4* v, int count);

	/**
	\brief Set matrices for a parameter

	\param[in] shader_type Type of shader
	\param[in] param Name of a parameter
	\param[in] mat Pointer to a matrices
	\param[in] count Number of matrices needed to be set

	\return Result of an operation
	*/
	void SetMatrix(Shader::Type shader_type, const char* param, Matrix* mat, int count);

	/**
	\brief Set texture for a parameter

	\param[in] shader_type Type of shader
	\param[in] param Name of a parameter
	\param[in] texture Pointer to a texture

	\return Result of an operation
	*/
	void SetTexture(Shader::Type shader_type, const char* param, Texture* texture);
};

CLASSFACTORYDEF(Program)
CLASSFACTORYDEF_END()
