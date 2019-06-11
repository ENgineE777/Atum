
#include "Services/Core/Core.h"

class AtumRunner : public Object
{
	bool inited = false;
	TaskExecutor::SingleTaskPool* renderTaskPool;

	void Draw(float dt);

public:

	void Init();
	void Resize(int width, int height);
	void Execute();
};

extern AtumRunner runner;