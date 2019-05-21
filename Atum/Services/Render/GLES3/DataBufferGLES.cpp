
#include "DataBufferGLES.h"

DataBufferGLES::DataBufferGLES(int sz, int strd) : DataBuffer(sz, strd)
{
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}

void* DataBufferGLES::Lock()
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	return (void*)glMapBufferRange(GL_ARRAY_BUFFER, 0, size, GL_MAP_WRITE_BIT);
}

void DataBufferGLES::Unlock()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void DataBufferGLES::Release()
{
	glDeleteBuffers(1, &buffer);
	delete this;
}