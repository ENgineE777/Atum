
#pragma once

#include "Services/Render/Device.h"

#ifdef PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

#ifdef PLATFORM_IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

/**
\ingroup gr_code_services_render_gles3
*/

class DeviceGLES : public Device
{
	friend class Render;
	friend class GeometryBufferGLES;
	friend class ShaderGLES;
	friend class TextureGLES;
	friend class VertexDeclGLES;

	class VertexDeclGLES* cur_vdecl = nullptr;
	class GeometryBufferGLES* cur_buff = nullptr;
	GLuint ibuff_type = 0;
	class GeometryBufferGLES* cur_ibuff = nullptr;

	bool need_apply_vdecl = false;

	bool vp_was_setted = false;
	Viewport cur_viewport;
	int  cur_depth_w;
	int  cur_depth_h;
	GLuint frame_buffer = 0;
	bool need_set_rt = false;
	bool use_def_backbuffer = true;
	class TextureGLES* cur_rt[6];
	class TextureGLES* cur_depth = nullptr;

	DeviceGLES();
	virtual bool Init(void* data);
	virtual void PrepareProgram(Program* program);
	virtual void Release();

	virtual Shader* CreateShader(Shader::Type type, const char* name);
	int GetBlendArg(BlendArg arg);
	int GetBlendOp(BlendOp op);
	int GetCompareFunc(CompareFunc func);

	void UpdateStates();

public:

	static DeviceGLES* instance;

	virtual void  SetVideoMode(int wgt, int hgt, void* data);
	virtual int   GetWidth();
	virtual int   GetHeight();
	virtual float GetAspect();

	virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue);
	virtual void Present();

	virtual void SetProgram(Program* program);

	virtual VertexDecl* CreateVertexDecl(int count, VertexDecl::ElemDesc* elems);
	virtual void SetVertexDecl(VertexDecl* vdecl);

	virtual GeometryBuffer* CreateBuffer(int count, int stride);
	virtual void SetVertexBuffer(int slot, GeometryBuffer* buffer);
	virtual void SetIndexBuffer(GeometryBuffer* buffer);

	virtual Texture* CreateTexture(int w, int h, Texture::Format f, int l, bool rt, Texture::Type tp);

	int GetPrimitiveType(Primitive type);
	int CalcPrimCount(Primitive type, int primCount);
	virtual void Draw(Primitive prim, int startVertex, int primCount);
	virtual void DrawIndexed(Primitive prim, int startVertex, int startIndex, int primCount);

	virtual void SetAlphaBlend(bool enable);
	virtual void SetBlendFunc(BlendArg src, BlendArg dest);
	virtual void SetBlendOperation(BlendOp op);
	virtual void SetDepthTest(bool enable);
	virtual void SetDepthWriting(bool enable);
	virtual void SetDepthFunc(CompareFunc func);
	virtual void SetCulling(CullMode mode);
	virtual void SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset);

	virtual void SetViewport(const Viewport& viewport);
	virtual void GetViewport(Viewport& viewport);

	virtual void SetRenderTarget(int slot, Texture* rt);
	virtual void SetDepth(Texture* depth);
	virtual void RestoreRenderTarget();
};
