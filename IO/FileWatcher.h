//
// Created by LIUHAO on 2017/12/16.
//

#ifndef URHO3DCOPY_FILEWATCHER_H
#define URHO3DCOPY_FILEWATCHER_H

#include "../Core/Thread.h"
#include "../Core/Object.h"
#include "FileSystem.h"
#include "../Core/Mutex.h"
#include "../Core/Timer.h"

namespace Urho3D
{
	class FileWatcher : public Object, public Thread
	{
		URHO3D_OBJECT(FileWatcher, Object);

	public:
		FileWatcher(Context* context);

		virtual ~FileWatcher() override ;

		void ThreadFunction() override;

		bool StartWatching(const String& pathName, bool watchSubDirs);
		void StopWatching();
		void SetDelay(float interval);
		void AddChange(const String& fileName);
		bool GetNextChange(String& dest);

		const String& GetPath() const { return path_; }

	private:
		SharedPtr<FileSystem> fileSystem_;
		String path_;
		HashMap<String, Timer> changes_;
		Mutex changesMutex_;
		float delay_;
		bool watchSubDirs_;
#ifdef _WIN32
		void* dirHandle_;
#elif __linux__
	//todo
#endif
	};

}



#endif //URHO3DCOPY_FILEWATCHER_H
