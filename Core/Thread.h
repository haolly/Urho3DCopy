//
// Created by LIUHAO on 2017/12/4.
//

#ifndef URHO3DCOPY_THREAD_H
#define URHO3DCOPY_THREAD_H

#ifndef _WIN32
#include <pthread.h>
using ThreaID = pthread_t;
#else
using ThreadID = unsigned;
#endif

namespace Urho3D
{
	class Thread
	{
	public:
		Thread();
		virtual ~Thread();

		virtual void ThreadFunction() = 0;

		bool Run();
		void Stop();
		void SetPriority(int priority);

		bool IsStarted() const { return handle_ != nullptr; }

		static void SetMainThread();
		static ThreadID GetCurrentThreadID();
		static bool IsMainThread();

	private:
		void* handle_;
		volatile bool shouldRun_;
		static ThreadID mainThreadID;
	};
}



#endif //URHO3DCOPY_THREAD_H
