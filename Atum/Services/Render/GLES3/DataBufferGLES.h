
#pragma once

#include "Services/Render/DataBuffer.h"
#include <vector>

#ifdef PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

#ifdef PLATFORM_IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

class DataBufferGLES : public DataBuffer
{
public:

	GLuint buffer;

	DataBufferGLES(int sz, int strd);

	virtual void* Lock();
	virtual void Unlock();
	virtual void Release();
};
