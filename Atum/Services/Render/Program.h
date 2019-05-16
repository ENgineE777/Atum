
#pragma once

#include "Shader.h"
#include "Texture.h"
#include <map>
#include "Support/ClassFactory.h"

/**
\ingroup gr_code_services_render
*/

/**
\brief PhysScene

This is representation of a physical scene. Objecte to a scene are adding via proper
methods of PhysScene.

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

	enum Stage
	{
		Vertex /*!< Hardware button in pressed state */,
		Pixel /*!< Hardware button in pressed state */
	};

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
	int program = -1;
#endif

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	static void SetVector(const char* param, Vector4* v);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	static void SetMatrix(const char* param, Matrix* mat);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	static void SetTexture(const char* param, Texture* texture);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual bool Init();

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual const char* GetVsName() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual const char* GetPsName() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void ApplyStates() {};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void SetVector(Stage stage, const char* param, Vector4* v, int count);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void SetMatrix(Stage stage, const char* param, Matrix* mat, int count);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void SetTexture(Stage stage, const char* param, Texture* texture);
};

CLASSFACTORYDEF(Program)
CLASSFACTORYDEF_END()
