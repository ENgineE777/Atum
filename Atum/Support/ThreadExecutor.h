
#pragma once

#include <windows.h>

class CriticalSection
{
public:

	CriticalSection();
	~CriticalSection();
	void Enter();
	void UnLock();

private:
	CRITICAL_SECTION critSection;
};

class ThreadExecutor
{
	enum State
	{
		Idle,
		Working,
		Finishing
	};

	State state = Idle;
	HANDLE thread = 0;

	static DWORD WINAPI Entry(void* arg);

public:

	void Execute();
	bool IsExecuting();
	void Terminate();

	virtual void Work() = 0;

	static void Sleep(int mili_sec);
};