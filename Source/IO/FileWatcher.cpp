//
// Created by LIUHAO on 2017/12/16.
//

#include "FileWatcher.h"
#include "Log.h"
#include "File.h"
#include "../Math/MathDefs.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
//todo
#endif

namespace Urho3D
{
#ifndef __APPLE__
	static const unsigned BUFFERSIZE = 4096;
#endif

	FileWatcher::FileWatcher(Context *context) :
			Object(context),
			fileSystem_(GetSubsystem<FileSystem>()),
			delay_(1.0f),
			watchSubDirs_(false)
	{
//todo
	}

	FileWatcher::~FileWatcher()
	{
		StopWatching();
		//todo
	}

	bool FileWatcher::StartWatching(const String &pathName, bool watchSubDirs)
	{
		if(!fileSystem_)
		{
			URHO3D_LOGERROR("No FileSystem, can not start watching");
			return false;
		}

		StopWatching();
#if defined(URHO3D_FILEWATCHER) && defined(URHO3D_THREADING)
#ifdef __WIN32
		String nativePath = GetNativePath(RemoveTrailingSlash(pathName));
		dirHandle_ = (void*)CreateFileW(
				WString(nativePath).CString(),
				FILE_LIST_DIRECTORY,
				FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
				nullptr,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS,
				nullptr
		);
		if(dirHandle_ != INVALID_HANDLE_VALUE)
		{
			path_ = AddTrailingSlash(pathName);
			watchSubDirs_ = watchSubDirs;
			Run();

			URHO3D_LOGDEBUG("Started watching path " + pathName);
			return true;
		} else {
			URHO3D_LOGERROR("Failed to start watching path " + pathName);
			return false;
		}

#elif defined(__linux__)
		//todo
#endif
#else
		URHO3D_LOGDEBUG("FileWatcher feature not enabled");
		return false;
#endif
	}

	void FileWatcher::StopWatching()
	{
		if(handle_)
		{
			shouldRun_ = false;

#ifdef _WIN32
			// Create and delete a dummy file to make sure the watcher loop terminates
			// This is only required on Windows platform
			//todo why do this ??
			String dummyFileName = path_ + "dummy.tmp";
			File file(context_, dummyFileName, FILE_WRITE);
			file.Close();
			if(fileSystem_)
				fileSystem_->Delete(dummyFileName);
#endif
#if defined(__APPLE__) && !defined(IOS) && !defined(TVOS)
			// Our implementation of file watcher requires the thread to be stopped first before closing the watcher
        Stop();
#endif

#ifdef _WIN32
			CloseHandle((HANDLE)dirHandle_);
#elif defined(__linux__)
			//todo
#endif
#ifndef __APPLE__
			Stop();
#endif
			URHO3D_LOGDEBUG("Stopped watching path" + path_);
			path_.Clear();
		}
	}

	void FileWatcher::SetDelay(float interval)
	{
		delay_ = Max(interval, 0.0f);
	}

	void FileWatcher::ThreadFunction()
	{
#ifdef URHO3D_FILEWATCHER
#ifdef _WIN32
		unsigned char buffer[BUFFERSIZE];
		DWORD  bytesFilled = 0;
		while(shouldRun_)
		{
			if(ReadDirectoryChangesW((HANDLE)dirHandle_,
			                         buffer,
			                         BUFFERSIZE,
			                         watchSubDirs_,
			                         FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
			                         &bytesFilled,
			                         nullptr,
			                         nullptr))
			{
				unsigned offset = 0;

				while(offset < bytesFilled)
				{
					FILE_NOTIFY_INFORMATION* record = (FILE_NOTIFY_INFORMATION*)&buffer[offset];
					if(record->Action == FILE_ACTION_MODIFIED || record->Action == FILE_ACTION_RENAMED_NEW_NAME)
					{
						String fileName;
						const wchar_t* src = record->FileName;
						const wchar_t* end = src + record->FileNameLength / 2;
						while(src < end)
							fileName.AppendUTF8(String::DecodeUTF16(src));
						fileName = GetInternalPath(fileName);
						AddChange(fileName);
					}
					if(!record->NextEntryOffset)
						break;
					else
						offset += record->NextEntryOffset;
				}
			}
		}
#elif __linux__
		//todo
#endif
	}


	void FileWatcher::AddChange(const String &fileName)
	{
		MutexLock lock(changesMutex_);
		changes_[fileName].Reset();
	}

	bool FileWatcher::GetNextChange(String &dest)
	{
		MutexLock lock(changesMutex_);

		unsigned delayMsec = (unsigned)(delay_ * 1000.0f);
		if(changes_.Empty())
			return false;
		else
		{
			for(auto it = changes_.Begin(); it != changes_.End(); ++it)
			{
				if(it->second_.GetMSec(false) >= delayMsec)
				{
					dest = it->first_;
					changes_.Erase(it);
					return true;
				}
			}
			return false;
		}
	}

}

