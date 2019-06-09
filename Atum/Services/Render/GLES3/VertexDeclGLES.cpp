
#include "VertexDeclGLES.h"
#include "DataBufferGLES.h"
#include "DeviceGLES.h"

VertexDeclGLES::VertexDeclGLES(int count, VertexDecl::ElemDesc* elems)
{
	int offset = 0;

	GLuint types[] = { GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT,
	                   GL_INT, GL_INT, GL_INT, GL_INT,
	                   GL_UNSIGNED_INT, GL_UNSIGNED_INT, GL_UNSIGNED_INT, GL_UNSIGNED_INT,
	                   GL_UNSIGNED_BYTE };

	uint8_t offsets[] = { 4, 8, 12, 16, 4, 8, 12, 16, 4, 8, 12, 16, 4 };

	int components[] = { 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 4 };

	attribs.resize(count);

	for (int i = 0; i< count; i++)
	{
		VertexAttrib& attr = attribs[i];

		int index = elems[i].type;

		attr.type = types[index];
		attr.offset = offset;
		attr.components = components[index];

		offset += offsets[index];
	}
}

void VertexDeclGLES::Apply()
{
	if (!DeviceGLES::instance->cur_buff)
	{
		return;
	}

	int stride = DeviceGLES::instance->cur_buff->GetStride();

	for (int i = 0; i < attribs.size(); i++)
	{
		VertexAttrib& attr = attribs[i];
		glVertexAttribPointer(i, attr.components, attr.type, (attr.type == GL_UNSIGNED_BYTE) ? GL_TRUE : GL_FALSE, stride, (GLvoid*)attr.offset);
		glEnableVertexAttribArray(i);
	}
}

void VertexDeclGLES::Release()
{
	delete this;
}