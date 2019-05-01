
#pragma once

#ifdef PLATFORM_PC
#include <windows.h>
#endif

/**
\ingroup gr_code_common
*/

class CriticalSection
{
public:

	CriticalSection();
	~CriticalSection();
	void Enter();
	void UnLock();

private:
#ifdef PLATFORM_PC
	CRITICAL_SECTION critSection;
#endif
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

#ifdef PLATFORM_PC
	HANDLE thread = 0;

	static DWORD WINAPI Entry(void* arg);
#endif

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