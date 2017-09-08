
#include "ThreadExecutor.h"
#include <windows.h>

CriticalSection::CriticalSection()
{
	InitializeCriticalSection(&critSection);
}

CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(&critSection);
}

void CriticalSection::Enter()
{
	EnterCriticalSection(&critSection);
}

void CriticalSection::UnLock()
{
	LeaveCriticalSection(&critSection);
}

void ThreadExecutor::Execute(ThreadCaller* caller, ThreadCaller::Delegate call)
{
	this->caller = caller;
	this->call = call;

	if (state != Idle)
	{
		return;
	}

	thread = CreateThread (0, 256*1024, Entry, this, CREATE_SUSPENDED, NULL);
	SetThreadPriority(thread, THREAD_PRIORITY_NORMAL);

	state = Working;
	ResumeThread(thread);
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
	::Sleep(mili_sec);
}

DWORD WINAPI ThreadExecutor::Entry(void* arg)
{
	ThreadExecutor* executor = (ThreadExecutor*)arg;
	(executor->caller->*executor->call)();
	executor->state = Idle;

	return 0;
}