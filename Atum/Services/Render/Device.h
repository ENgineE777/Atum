
#pragma once

#include "Support/Support.h"
#include "Shader.h"
#include "GeometryBuffer.h"
#include "Program.h"
#include "VertexDecl.h"
#include "Texture.h"

/**
\ingroup gr_code_services_render
*/

/**
\brief PhysScene

This is representation of a physical scene. Objecte to a scene are adding via proper
methods of PhysScene.

*/

class Device
{
	friend class Render;
	friend class Program;
	friend class DeviceDX11;
	friend class DeviceGLES;

	int  scr_w = 0;
	int  scr_h = 0;
	int  cur_rt_w = 0;
	int  cur_rt_h = 0;
	float cur_aspect = 0.0f;

	virtual bool Init(void* external_device) = 0;
	virtual void PrepareProgram(Program* program) = 0;
	virtual void Release() = 0;

	Program* cur_program = nullptr;

public:

	struct Viewport
	{
		int x, y;
		int width, height;
		float minZ, maxZ;
	};

	struct Rect
	{
		int left, right;
		int top, bottom;
	};

	enum Primitive
	{
		LineStrip = 0 /*!< Hardware button in pressed state */,
		LinesList /*!< Hardware button in pressed state */,
		TriangleStrip /*!< Hardware button in pressed state */,
		TrianglesList /*!< Hardware button in pressed state */
	};

	enum CullMode
	{
		CullNone = 0 /*!< Hardware button in pressed state */,
		CullCW /*!< Hardware button in pressed state */,
		CullCCW /*!< Hardware button in pressed state */
	};

	enum BlendOp
	{
		BlendAdd = 0 /*!< Hardware button in pressed state */,
		BlendSub /*!< Hardware button in pressed state */,
		BlendRevSub /*!< Hardware button in pressed state */,
		BlendMin /*!< Hardware button in pressed state */,
		BlendMax /*!< Hardware button in pressed state */
	};

	enum CompareFunc
	{
		CmpNever = 0 /*!< Hardware button in pressed state */,
		CmpLess /*!< Hardware button in pressed state */,
		CmpEqual /*!< Hardware button in pressed state */,
		CmpLessEqual /*!< Hardware button in pressed state */,
		CmpGreater /*!< Hardware button in pressed state */,
		CmpNotequal /*!< Hardware button in pressed state */,
		CmpGreaterqual /*!< Hardware button in pressed state */,
		CmpAlways /*!< Hardware button in pressed state */
	};

	enum BlendArg
	{
		ArgZero = 0 /*!< Hardware button in pressed state */,
		ArgOne /*!< Hardware button in pressed state */,
		ArgSrcColor /*!< Hardware button in pressed state */,
		ArgInvSrcColor /*!< Hardware button in pressed state */,
		ArgSrcAlpha /*!< Hardware button in pressed state */,
		ArgInvSrcAlpha /*!< Hardware button in pressed state */,
		ArgDestAlpha /*!< Hardware button in pressed state */,
		ArgInvDestAlpha /*!< Hardware button in pressed state */,
		ArgDestColor /*!< Hardware button in pressed state */,
		ArgInvDestColor /*!< Hardware button in pressed state */
	};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void  SetVideoMode(int wgt, int hgt, void* data) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void* GetBackBuffer() { return nullptr; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual int   GetWidth() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual int   GetHeight() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual float GetAspect() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Present() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetProgram(Program* program) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual VertexDecl* CreateVertexDecl(int count, VertexDecl::ElemDesc* elems) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetVertexDecl(VertexDecl* vdecl) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual GeometryBuffer* CreateBuffer(int count, int stride) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetVertexBuffer(int slot, GeometryBuffer* buffer) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetIndexBuffer(GeometryBuffer* buffer) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual Texture* CreateTexture(int w, int h, Texture::Format f, int l, bool rt, Texture::Type tp) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Draw(Primitive prim, int startVertex, int primCount) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void DrawIndexed(Primitive prim, int startVertex, int startIndex, int primCount) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetAlphaBlend(bool enable) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetBlendFunc(BlendArg src, BlendArg dest) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetBlendOperation(BlendOp op) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetDepthTest(bool enable) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetDepthWriting(bool enable) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetDepthFunc(CompareFunc func) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetCulling(CullMode mode) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetViewport(const Viewport& viewport) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void GetViewport(Viewport& viewport) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetRenderTarget(int slot, Texture* rt) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetDepth(Texture* depth) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void RestoreRenderTarget() = 0;

protected:
	virtual Shader* CreateShader(Shader::Type type, const char* name) = 0;
};