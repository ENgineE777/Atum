#pragma once

class block_allocator
{
private:
	struct block
	{
		int size;
		int used;
		char *buffer;
		block *next;
	};

	block *m_head;
	int m_blocksize;

	block_allocator(const block_allocator &);
	block_allocator &operator=(block_allocator &);

public:
	block_allocator(int blocksize);
	~block_allocator();

	// exchange contents with rhs
	void swap(block_allocator &rhs);

	// allocate memory
	void *malloc(int size);

	// free all allocated blocks
	void free();
};