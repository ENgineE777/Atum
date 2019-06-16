
#include "DeviceGLES.h"
#include "ShaderGLES.h"
#include "DataBufferGLES.h"
#include "VertexDeclGLES.h"
#include "TextureGLES.h"
#include "Services/Core/Core.h"

DeviceGLES* DeviceGLES::instance = nullptr;

DeviceGLES::DeviceGLES()
{
	instance = this;

	for (int i = 0; i < 6; i++)
	{
		cur_rt[i] = nullptr;
	}
}

bool DeviceGLES::Init(void* data)
{
	core.Log("Render", "Version %i", GL_VERSION);
	core.Log("Render", "Vendor %i", GL_VENDOR);
	core.Log("Render", "Renderer %i", GL_RENDERER);
	core.Log("Render", "Extensions %i", GL_EXTENSIONS);

	glGenFramebuffers(1, &frame_buffer);

	return true;
}

void DeviceGLES::SetVideoMode(int wgt, int hgt, void* data)
{
    if (scr_w != wgt && scr_h != hgt)
    {
        core.Log("Render", "Set videomode : %i x %i", wgt, hgt);
    }

	scr_w = wgt;
	scr_h = hgt;

	cur_aspect = (float)hgt / (float)wgt;

	glViewport(0, 0, scr_w, scr_h);
}

int DeviceGLES::GetWidth()
{
	return scr_w;
}

int DeviceGLES::GetHeight()
{
	return scr_h;
}

float DeviceGLES::GetAspect()
{
	return cur_aspect;
}

void DeviceGLES::Clear(bool renderTarget, Color color, bool zbuffer, float zValue)
{
	UpdateStates();

	glClearColor(color.r, color.g, color.b, color.a);
	glClearDepthf(zbuffer);

	glClear((renderTarget ? GL_COLOR_BUFFER_BIT : 0) | (zbuffer ? GL_DEPTH_BUFFER_BIT : 0));
}

void DeviceGLES::Present()
{
}

void DeviceGLES::PrepareProgram(Program* program)
{
	program->program = glCreateProgram();

	if (program->vshader)
	{
		((ShaderGLES*)program->vshader)->program = program->program;
		glAttachShader(program->program, ((ShaderGLES*)program->vshader)->shader);
	}

	if (program->pshader)
	{
		((ShaderGLES*)program->pshader)->program = program->program;
		glAttachShader(program->program, ((ShaderGLES*)program->pshader)->shader);
	}

	glLinkProgram(program->program);

	GLint linkSuccess;
	glGetProgramiv(program->program, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE)
	{
		GLchar messages[256];
		glGetProgramInfoLog(program->program, sizeof(messages), 0, &messages[0]);
		core.Log("Render", "Error in linking program: %s",messages);
	}
}

void DeviceGLES::SetProgram(Program* program)
{
	if (cur_program != program)
	{
		cur_program = program;

		SetAlphaBlend(false);
		SetDepthTest(true);
		SetDepthWriting(true);

		if (cur_program)
		{
			glUseProgram(cur_program->program);
			cur_program->ApplyStates();
		}
	}
}

VertexDecl* DeviceGLES::CreateVertexDecl(int count, VertexDecl::ElemDesc* elems)
{
	return new VertexDeclGLES(count, elems);
}

void DeviceGLES::SetVertexDecl(VertexDecl* vdecl)
{
	if (cur_vdecl != vdecl)
	{
		cur_vdecl = (VertexDeclGLES*)vdecl;

		if (cur_vdecl)
		{
			need_apply_vdecl = true;
		}
	}

}

DataBuffer* DeviceGLES::CreateBuffer(int count, int stride)
{
	return new DataBufferGLES(count, stride);
}

void DeviceGLES::SetVertexBuffer(int slot, DataBuffer* buffer)
{
	if (cur_buff == buffer)
	{
		return;
	}

	cur_buff = (DataBufferGLES*)buffer;

	if (cur_buff)
	{
		if (cur_vdecl)
		{
			need_apply_vdecl = true;
		}

		glBindBuffer(GL_ARRAY_BUFFER, cur_buff->buffer);
	}
}

void DeviceGLES::SetIndexBuffer(DataBuffer* buffer)
{
	if (cur_ibuff == buffer)
	{
		return;
	}

	cur_ibuff = (DataBufferGLES*)buffer;

	if (cur_ibuff)
	{
		ibuff_type = (buffer->GetStride() == 4) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cur_ibuff->buffer);
	}
}

Shader* DeviceGLES::CreateShader(Shader::Type type, const char* name)
{
	return new ShaderGLES(type, name);
}

Texture* DeviceGLES::CreateTexture(int w, int h, Texture::Format f, int l, bool rt, Texture::Type tp)
{
	return new TextureGLES(w, h, f, l, rt, tp);
}

int DeviceGLES::GetPrimitiveType(Primitive type)
{
	int tp = GL_TRIANGLES;
	switch (type)
	{
		case LineStrip:
		{
			tp = GL_LINE_STRIP;
		}
		break;
		case LinesList:
		{
			tp = GL_LINES;
		}
		break;
		case TriangleStrip:
		{
			tp = GL_TRIANGLE_STRIP;
		}
		break;
		case TrianglesList:
		{
			tp = GL_TRIANGLES;
		}
		break;
	}

	return tp;
}

int DeviceGLES::CalcPrimCount(Primitive type, int primCount)
{
	int tp = primCount;

	switch (type)
	{
		case LineStrip:
		{
			tp = primCount + 1;
		}
		break;
		case LinesList:
		{
			tp = primCount * 2;
		}
		break;
		case TriangleStrip:
		{
			tp = primCount + 2;
		}
		break;
		case TrianglesList:
		{
			tp = primCount * 3;
		}
		break;
	}

	return tp;
}

void DeviceGLES::Draw(Primitive prim, int startVertex, int primCount)
{
	UpdateStates();

	glDrawArrays(GetPrimitiveType(prim), startVertex, CalcPrimCount(prim, primCount));
}

void DeviceGLES::DrawIndexed(Primitive prim, int startVertex, int startIndex, int primCount)
{
	if (!cur_ibuff)
	{
		return;
	}

	UpdateStates();

	glDrawElements(GetPrimitiveType(prim), CalcPrimCount(prim, primCount), ibuff_type, nullptr);
}

void DeviceGLES::UpdateStates()
{
	if (need_set_rt)
	{
		if (use_def_backbuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, scr_w, scr_h);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

			for (int i = 0; i<1;i++)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, cur_rt[i] ? cur_rt[i]->texture : 0, 0);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, cur_depth ? cur_depth->texture : 0, 0);

			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
				core.Log("Render", "glBindFramebuffer error - %i", status);
			}

			glViewport(0, 0, cur_rt_w, cur_rt_h);
		}

		need_set_rt = false;
	}

	if (need_apply_vdecl)
	{
		cur_vdecl->Apply();
		need_apply_vdecl = false;
	}
}

void DeviceGLES::SetAlphaBlend(bool enable)
{
	if (enable>0)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

int DeviceGLES::GetBlendArg(BlendArg arg)
{
	int arg_op = 0;

	switch (arg)
	{
		case ArgZero:
			arg_op = GL_ZERO;
			break;
		case ArgOne:
			arg_op = GL_ONE;
			break;
		case ArgSrcColor:
			arg_op = GL_SRC_COLOR;
			break;
		case ArgInvSrcColor:
			arg_op = GL_ONE_MINUS_SRC_COLOR;
			break;
		case ArgSrcAlpha:
			arg_op = GL_SRC_ALPHA;
			break;
		case ArgInvSrcAlpha:
			arg_op = GL_ONE_MINUS_SRC_ALPHA;
			break;
		case ArgDestAlpha:
			arg_op = GL_DST_ALPHA;
			break;
		case ArgInvDestAlpha:
			arg_op = GL_ONE_MINUS_DST_ALPHA;
			break;
		case ArgDestColor:
			arg_op = GL_DST_COLOR;
			break;
		case ArgInvDestColor:
			arg_op = GL_ONE_MINUS_DST_COLOR;
			break;
	}

	return arg_op;
}

int DeviceGLES::GetCompareFunc(CompareFunc func)
{
	int cmp = 0;

	switch (func)
	{
		case CmpNever:
			cmp = GL_NEVER;
			break;
		case CmpLess:
			cmp = GL_LESS;
			break;
		case CmpEqual:
			cmp = GL_EQUAL;
			break;
		case CmpLessEqual:
			cmp = GL_LEQUAL;
			break;
		case CmpGreater:
			cmp = GL_GREATER;
			break;
		case CmpNotequal:
			cmp = GL_NOTEQUAL;
			break;
		case CmpGreaterqual:
			cmp = GL_GEQUAL;
			break;
		case CmpAlways:
			cmp = GL_ALWAYS;
			break;
	}

	return cmp;
}

int DeviceGLES::GetBlendOp(BlendOp op)
{
	int blend_op = 0;

	switch (op)
	{
		case BlendAdd:
			blend_op = GL_FUNC_ADD;
			break;
		case BlendSub:
			blend_op = GL_FUNC_SUBTRACT;
			break;
		case BlendMin:
			blend_op = GL_MIN;
			break;
		case BlendMax:
			blend_op = GL_MAX;
			break;
		case BlendRevSub:
			blend_op = GL_FUNC_REVERSE_SUBTRACT;
			break;
	}

	return blend_op;
}

void DeviceGLES::SetBlendFunc(BlendArg src, BlendArg dest)
{
	glBlendFunc(GetBlendArg(src), GetBlendArg(dest));
}

void DeviceGLES::SetBlendOperation(BlendOp op)
{
	glBlendEquation(GetBlendOp(op));
}

void DeviceGLES::SetDepthTest(bool enable)
{
	if (enable>0)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void DeviceGLES::SetDepthWriting(bool enable)
{
	if (enable>0)
	{
		glDepthMask(GL_TRUE);
	}
	else
	{
		//glDepthMask(GL_FALSE);
	}
}

void DeviceGLES::SetDepthFunc(CompareFunc func)
{
	glDepthFunc(GetCompareFunc(func));
}

void DeviceGLES::SetCulling(CullMode mode)
{
	if (mode == CullNone)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);

		if (mode == CullCW)
		{
			glFrontFace(GL_CCW);
		}
		else
		{
			glFrontFace(GL_CW);
		}
	}
}

void DeviceGLES::SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset)
{
	if (enable)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(slopeZBias, depthOffset);
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void DeviceGLES::SetViewport(const Viewport& viewport)
{
	cur_viewport = viewport;
	glViewport(cur_viewport.x, cur_viewport.y, cur_viewport.width, cur_viewport.height);
	glDepthRangef(cur_viewport.minZ, cur_viewport.maxZ);

	vp_was_setted = true;
}

void DeviceGLES::GetViewport(Viewport& viewport)
{
	if (vp_was_setted)
	{
		viewport.width = cur_viewport.width;
		viewport.height = cur_viewport.height;
		viewport.x = cur_viewport.x;
		viewport.y = cur_viewport.y;
		viewport.minZ = cur_viewport.minZ;
		viewport.maxZ = cur_viewport.maxZ;
	}
	else
	{
		viewport.x = 0;
		viewport.y = 0;

		if (cur_rt[0])
		{
			viewport.width = cur_rt_w;
			viewport.height = cur_rt_h;
		}
		else
		{
			viewport.width = cur_depth_w;
			viewport.height = cur_depth_h;
		}

		viewport.minZ = 0.0f;
		viewport.maxZ = 1.0f;
	}
}

void DeviceGLES::SetRenderTarget(int slot, Texture* rt)
{
	vp_was_setted = false;
	need_set_rt = true;
	use_def_backbuffer = false;
	cur_rt[slot] = rt ? (TextureGLES*)rt : nullptr;

	if (slot == 0)
	{
		cur_rt_w = rt ? rt->GetWidth() : -1;
		cur_rt_h = rt ? rt->GetHeight() : -1;
	}
}

void DeviceGLES::SetDepth(Texture* depth)
{
	vp_was_setted = false;
	need_set_rt = true;
	use_def_backbuffer = false;
	cur_depth = depth ? (TextureGLES*)depth : nullptr;
	cur_depth_w = cur_depth ? cur_depth->GetWidth() : -1;
	cur_depth_h = cur_depth ? cur_depth->GetHeight() :-1;
}

void DeviceGLES::RestoreRenderTarget()
{
	vp_was_setted = false;
	cur_rt_w = scr_w;
	cur_rt_h = scr_h;

	need_set_rt = true;
	use_def_backbuffer = true;

	for (int i = 0; i < 6; i++)
	{
		cur_rt[i] = nullptr;
	}

	cur_depth = nullptr;
}

void DeviceGLES::Release()
{
	delete this;
}
