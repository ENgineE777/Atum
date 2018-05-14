
#include "ThreadExecutor.h"

CriticalSection::CriticalSection()
{
#ifdef PLATFORM_PC
	InitializeCriticalSection(&critSection);
#endif
}

CriticalSection::~CriticalSection()
{
#ifdef PLATFORM_PC
	DeleteCriticalSection(&critSection);
#endif
}

void CriticalSection::Enter()
{
#ifdef PLATFORM_PC
	EnterCriticalSection(&critSection);
#endif
}

void CriticalSection::UnLock()
{
#ifdef PLATFORM_PC
	LeaveCriticalSection(&critSection);
#endif
}

void ThreadExecutor::Execute(ThreadCaller* caller, ThreadCaller::Delegate call)
{
	this->caller = caller;
	this->call = call;

	if (state != Idle)
	{
		return;
	}

#ifdef PLATFORM_PC
	thread = CreateThread (0, 256*1024, Entry, this, CREATE_SUSPENDED, NULL);
	SetThreadPriority(thread, THREAD_PRIORITY_NORMAL);

	state = Working;
	ResumeThread(thread);
#endif
}

bool ThreadExecutor::IsExecuting()
{
	return (state != Idle);
}

void ThreadExecutor::Terminate()
{
	if (state != Working)
	{
		return;
	}

	state = Finishing;

	while (state == Finishing)
	{
		Sleep(20);
	}
}

void ThreadExecutor::Sleep(int mili_sec)
{
#ifdef PLATFORM_PC
	::Sleep(mili_sec);
#endif
}

#ifdef PLATFORM_PC
DWORD WINAPI ThreadExecutor::Entry(void* arg)
{
	ThreadExecutor* executor = (ThreadExecutor*)arg;
	(executor->caller->*executor->call)();
	executor->state = Idle;

	return 0;
}
#endif