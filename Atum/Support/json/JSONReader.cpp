
#include "JSONReader.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>

JSONReader::JSONReader() : allocator(1 << 10)
{
	root = NULL;
	curNode = NULL;
	curDepth = 0;

	buffer = NULL;
}

bool JSONReader::Parse(const char* name)
{
	FILE* file = fopen(name, "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = (uint8_t*)malloc(size + 1);
		fread(buffer, size, 1, file);

		buffer[size] = 0;

		fclose(file);
	}

	if (buffer)
	{
		int offset = 0;

		if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
		{
			offset = 3;
		}

		char *errorPos = 0;
		char *errorDesc = 0;
		int errorLine = 0;

		root = json_parse((char*)(buffer + offset), &errorPos, &errorDesc, &errorLine, &allocator);

		if (root)
		{
			nodes[curDepth] = root;
			curNode = nodes[curDepth];

			return true;
		}
	}

	return false;
}

bool JSONReader::EnterBlock(const char* name)
{
	nodes[curDepth +1] = FindValue(name);

	if (nodes[curDepth +1])
	{
		if (nodes[curDepth +1]->type == JSON_OBJECT)
		{
			curDepth++;
			curNode = nodes[curDepth];

			return true;
		}
		else
		if (nodes[curDepth +1]->type == JSON_ARRAY)
		{
			if (nodes[curDepth +1]->first_child)
			{
				curDepth++;
				nodes[curDepth] = nodes[curDepth]->first_child;
				curNode = nodes[curDepth];

				return true;
			}
		}
	}

	return false;
}

void JSONReader::LeaveBlock()
{
	if (curDepth == 0) return;

	if (nodes[curDepth]->parent->type == JSON_ARRAY)
	{
		nodes[curDepth]->parent->first_child = nodes[curDepth]->next_sibling;
	}
	
	curDepth--;
	curNode = nodes[curDepth];
}

bool JSONReader::Read(const char* name, char* val, int val_len)
{
	json_value* node = FindValue(name);
	
	if (node)
	{
		if (node->type == JSON_STRING)
		{
			strcpy(val, node->string_value);
			return true;
		}
		else
		if (node->type == JSON_BOOL || node->type == JSON_INT)
		{
			sprintf(val, "%i", node->int_value);
			return true;
		}
		else
		if (node->type == JSON_FLOAT)
		{
			sprintf(val, "%4.5f", node->float_value);
			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			sprintf(val, "");
			return true;
		}
	}

	return false;
}

bool JSONReader::Read(const char* name, std::string& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_STRING)
		{
			val = node->string_value;
			return true;
		}
		else
		if (node->type == JSON_BOOL || node->type == JSON_INT)
		{
			char tmp[128];
			sprintf(tmp, "%i", node->int_value);
			val = tmp;

			return true;
		}
		else
		if (node->type == JSON_FLOAT)
		{
			char tmp[128];
			sprintf(tmp, "%4.5f", node->float_value);
			val = tmp;

			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			val = "";
			return true;
		}
	}

	return false;
}

bool JSONReader::Read(const char* name, bool& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_BOOL)
		{
			if (node->int_value > 0)
			{
				val = true;
			}
			else
			{
				val = false;
			}

			return true;
		}
		else
		if (node->type == JSON_STRING)
		{
			if (atoi(node->string_value) > 0)
			{
				val = true;
			}
			else
			{
				val = false;
			}

			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			val = false;

			return true;
		}
	}

	return false;
}

bool JSONReader::Read(const char* name, float& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_FLOAT)
		{
			val = node->float_value;
			return true;
		}		
		else
		if (node->type == JSON_INT)
		{
			val = node->int_value;
			return true;
		}
		else
		if (node->type == JSON_STRING)
		{
			val = atof(node->string_value);
			return true;
		}
	}

	return false;
}

bool JSONReader::Read(const char* name, int& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_INT)
		{
			val = node->int_value;

			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			val = 0;

			return true;
		}
		else
		if (node->type == JSON_STRING)
		{
			val = atoi(node->string_value);
			return true;
		}
	}

	return false;
}

bool JSONReader::Read(const char* name, int64_t& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_INT)
		{
			val = node->int64_value;
			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			val = 0;

			return true;
		}
		else
		if (node->type == JSON_STRING)
		{
			val = atol(node->string_value);
			return true;
		}
	}

	return false;
}

bool JSONReader::Read(const char* name, Vector& val)
{
	if (EnterBlock(name))
	{
		Read("x", val.x);
		Read("y", val.y);
		Read("z", val.z);

		LeaveBlock();

		return true;
	}

	return false;
}

bool JSONReader::Read(const char* name, Matrix& val)
{
	if (EnterBlock(name))
	{
		for (int i = 0; i < 16; i++)
		{
			char tmp[16];
			sprintf(tmp, "m%i", i);
			Read(tmp, val.matrix[i]);
		}
		
		LeaveBlock();

		return true;
	}

	return false;
}

bool JSONReader::Read(const char* name, Color& val)
{
	if (EnterBlock(name))
	{
		Read("r", val.r);
		Read("g", val.g);
		Read("b", val.b);
		Read("a", val.a);

		LeaveBlock();

		return true;
	}

	return false;
}

json_value* JSONReader::FindValue(const char* name)
{
	if (!root)
	{
		return NULL;
	}

	if (!name[0])
	{
		return curNode;
	}

	for (json_value *it = curNode->first_child; it; it = it->next_sibling)
	{
		if (stricmp(name, it->name) == 0)
		{
			return it;
		}
	}

	return NULL;
}

void JSONReader::Release()
{
	if (buffer)
	{
		free(buffer);
	}

	allocator.free();

	delete this;
}
