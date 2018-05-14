
#pragma once

#include "Services/Render/GeometryBuffer.h"
#include <vector>

#ifdef PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

class GeometryBufferGLES : public GeometryBuffer
{
public:

	GLuint buffer;

	GeometryBufferGLES(int sz, int strd);

	virtual void* Lock();
	virtual void Unlock();
	virtual void Release();
};
