
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

class ThreadCaller
{
public:
	typedef void (ThreadCaller::*Delegate)();

	virtual ~ThreadCaller() {};
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

	typedef void (ThreadExecutor::*Delegate)(float timedelta);

	void Execute(ThreadCaller* caller, ThreadCaller::Delegate call);
	bool IsExecuting();
	void Terminate();

	static void Sleep(int mili_sec);

private:
	ThreadCaller* caller;
	ThreadCaller::Delegate call;
};