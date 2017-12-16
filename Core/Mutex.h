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
		MutexLock(Mutex& mutex);
		~MutexLock();

	private:
		//Note, prevent copy construction and copy assignment
		MutexLock(const MutexLock& rhs);
		MutexLock&operator =(const MutexLock& rhs);
		Mutex& mutex_;
	};

}



#endif //URHO3DCOPY_MUTEX_H
