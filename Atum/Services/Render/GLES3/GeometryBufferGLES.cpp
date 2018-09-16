
#include "GeometryBufferGLES.h"

GeometryBufferGLES::GeometryBufferGLES(int sz, int strd) : GeometryBuffer(sz, strd)
{
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}

void* GeometryBufferGLES::Lock()
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	return (void*)glMapBufferRange(GL_ARRAY_BUFFER, 0, size, GL_MAP_WRITE_BIT);
}

void GeometryBufferGLES::Unlock()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void GeometryBufferGLES::Release()
{
	glDeleteBuffers(1, &buffer);
	delete this;
}