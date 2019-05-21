
#pragma once

#include "Support/Support.h"

/**
\ingroup gr_code_services_render
*/

/**
\brief DataBuffer

Data buffer which can store gemotry, indices, constants.

*/

class DataBuffer
{
protected:

	int size;
	int stride;

public:

#ifndef DOXYGEN_SKIP

	DataBuffer(int count, int strd) { size = count * strd; stride = strd; };
	virtual ~DataBuffer() {};

#endif

	/**
	\brief Lock buffer of a writing

	\return Pointer for a writing data into buffer
	*/
	virtual void* Lock() = 0;

	/**
	\brief Unlock buffer after writing
	*/
	virtual void Unlock() = 0;

	/**
	\brief Get size in bytes

	\return size in bytes
	*/
	int GetSize() { return size; };

	/**
	\brief Get stride

	\return Stride
	*/
	int GetStride() { return stride; };

	/**
	\brief DataBuffer should released only via this method
	*/
	virtual void Release() = 0;
};
