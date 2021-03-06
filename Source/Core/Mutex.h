//
// Created by LIUHAO on 2017/12/13.
//

#ifndef URHO3DCOPY_MUTEX_H
#define URHO3DCOPY_MUTEX_H

namespace Urho3D
{
	class Mutex
	{
	public:
		Mutex();
		~Mutex();

		void Acquire();
		bool TryAcquire();
		void Release();

	private:
		void* handle_;
	};

	class MutexLock
	{
	public:
		explicit MutexLock(Mutex& mutex);
		~MutexLock();

		//Note, prevent copy construction and copy assignment
		MutexLock(const MutexLock& rhs) = delete;
		MutexLock&operator =(const MutexLock& rhs) = delete;

	private:
		Mutex& mutex_;
	};

}



#endif //URHO3DCOPY_MUTEX_H
