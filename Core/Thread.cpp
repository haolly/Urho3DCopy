//
// Created by LIUHAO on 2017/12/4.
//

#include "Thread.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace Urho3D
{
#ifdef URHO3D_THREADING
#ifdef _WIN32
	static DWORD WINAPI ThreadFunctionStatic(void* data)
	{
		Thread* thread = static_cast<Thread*>(data);
		thread->ThreadFunction();
		return 0;
	}
#else
	static void* ThreadFunctionStatic(void* data)
	{
		Thread* thread = static_cast<Thread*>(data);
		thread->ThreadFunction();
		pthread_exit((void*)0);
		return 0;
	}
#endif  //_WIN32
#endif //URHO3D_THREADING

ThreadID Thread::mainThreadID;

	Thread::Thread() :
		handle_(nullptr),
		shouldRun_(false)
	{
	}

	Thread::~Thread()
	{
		Stop();
	}

	bool Thread::Run()
	{
#ifdef URHO3D_THREADING
		if(handle_)
			return false;
		shouldRun_ = true;
#ifdef _WIN32
		handle_ = CreateThread(nullptr, 0, ThreadFunctionStatic, this, 0, nullptr);
#else
		handler_ = new pthread_t;
		pthread_attr_t type;
		pthread_attr_init(&type);
		pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);
		pthread_create((pthread_t*)handle_, &type, ThreadFunctionStatic, this);
#endif
		return handle_ != nullptr;
#else
		return false;
#endif
	}

	void Thread::Stop()
	{
		//todo

	}

	void Thread::SetMainThread()
	{
		mainThreadID = GetCurrentThreadID();
	}

bool Urho3D::Thread::IsMainThread()
{
#ifdef URHO3D_THREADING
	return GetCurrentThreadID() == mainThreadID;
#else
	return true;
#endif
}

ThreadID Thread::GetCurrentThreadID()
{
#ifdef URHO3D_THREADING
#ifdef _WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
#else
	return ThreaID();
#endif  //URHO3D_THREADING
}

}

