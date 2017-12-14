//
// Created by liuhao on 2017/12/9.
//

#include "FileSystem.h"
#include "../Core/Thread.h"
#include "../Math/MathDefs.h"
#include "../ThirdParty/SDL/include/SDL_stdinc.h"
#include "../ThirdParty/SDL/include/SDL_filesystem.h"
#include "File.h"
#include "../Core/CoreEvent.h"
#include "../Engine/EngineEvents.h"
#include "Log.h"
#include "../Core/StringUtils.h"
#include "IOEvent.h"

#ifdef _WIN32
#include <winbase.h>
#include <winuser.h>
#include <shellapi.h>
#else
#endif

extern "C"
{
//todo
}


namespace Urho3D
{
	int DoSystemCommand(const String& commandLine, bool redirectToLog, Context* context)
	{
#if defined(TVOS) || defined(IOS)
		return -1;
#else
		if(!redirectToLog)
		{
			return system(commandLine.CString());
		}
		String stderrFilename;
		String adjustedCommandLine(commandLine);
		char* prefPath = SDL_GetPrefPath("urho3d", "temp");
		if(prefPath)
		{
			stderrFilename = String(prefPath) + "command-stderr";
			adjustedCommandLine += " 2>" + stderrFilename;
			SDL_free(prefPath);
		}
		FILE* file = popen(adjustedCommandLine.CString(), "r");
		if(!file)
			return -1;

		char buffer[128];
		while (!feof(file))
		{
			if(fgets(buffer, sizeof(buffer), file))
			{
				URHO3D_LOGRAW(String(buffer));
			}
		}
		int exitCode = pclose(file);
		if(!stderrFilename.Empty())
		{
			SharedPtr<File> errFile(new File(context, stderrFilename, FILE_READ));
			while(!errFile->IsEof())
			{
				unsigned numread = errFile->Read(buffer, sizeof(buffer));
				if(numread)
					Log::WriteRaw(String(buffer, numread), true);
			}
		}
		return exitCode;
#endif
	}

	int DoSystemRun(const String& fileName, const Vector<String>& arguments)
	{
		String fixedFileName = GetNativePath(fileName);
#ifdef _WIN32
		if(GetExtension(fixedFileName).Empty())
			fixedFileName += ".exe";
		String commandLine = "\"" + fixedFileName + "\"";
		for(unsigned i = 0; i<arguments.Size(); ++i)
			commandLine += " " + arguments[i];

		STARTUPINFOW startupInfo;
		PROCESS_INFORMATION processInfo;
		memset(&startupInfo, 0, sizeof(startupInfo));
		memset(&processInfo, 0, sizeof(processInfo));

		WString commandLineW(commandLine);
		if(!CreateProcessW(nullptr, (wchar_t*)commandLineW.CString(), nullptr, nullptr, 0, CREATE_NO_WINDOW, nullptr,
		                   nullptr, &startupInfo, &processInfo))
		{
			return -1;
		}
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		DWORD exitCode;
		GetExitCodeProcess(processInfo.hProcess, &exitCode);

		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		return exitCode;
#else
		//todo
#endif
	}

	class AsyncExecRequest : public Thread
	{
	public:
		AsyncExecRequest(unsigned& requestID) :
			requestID_(requestID),
			completed_(false)
		{
			//Increment ID for next request
			++requestID;
			if(requestID == M_MAX_UNSIGNED)
				requestID = 1;
		}

		unsigned GetRequestID() const { return requestID_; }
		int GetExitCode() const { return exitCode_; }
		bool IsCompleted() const { return completed_; }

	protected:
		unsigned requestID_;
		int exitCode_;
		volatile bool completed_;
	};

	class AsyncSystemCommand : public AsyncExecRequest
	{
	public:
		AsyncSystemCommand(unsigned requestID, const String& commandLine) :
			AsyncExecRequest(requestID),
			commandLine_(commandLine)
		{
			Run();
		}

		virtual void ThreadFunction() override
		{
			exitCode_ = DoSystemCommand(commandLine_, false, nullptr);
			completed_ = true;
		}

	private:
		String commandLine_;
	};

	class AsyncSystemRun : public AsyncExecRequest
	{
	public:
		AsyncSystemRun(unsigned int &requestID, const String& fileName, const Vector<String>& arguments) :
				AsyncExecRequest(requestID),
				fileName_(fileName),
				arguments_(arguments)
		{
		}

		void ThreadFunction() override
		{
			exitCode_ = DoSystemRun(fileName_, arguments_);
			completed_ = true;
		}

	private:
		String fileName_;
		const Vector<String>& arguments_;
	};

	FileSystem::FileSystem(Context *context) :
			Object(context),
			nextAsyncExecID_(1),
			executeConsoleCommands_(false)
	{
		SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(FileSystem, HandleBeginFrame));
		SetExecuteConsoleCommands(true);
	}

	FileSystem::~FileSystem()
	{
		if(asyncExecQueue_.Size())
		{
			for(auto it = asyncExecQueue_.Begin(); it != asyncExecQueue_.End(); ++it)
				delete(*it);
			asyncExecQueue_.Clear();
		}
	}

	bool FileSystem::SetCurrentDir(const String &pathName)
	{
		if(!CheckAccess(pathName))
		{
			URHO3D_LOGERROR("Access denied to " + pathName);
			return false;
		}
#ifdef _WIN32
		if(SetCurrentDirectoryW(GetWideNativePath(pathName).CString()) == FALSE)
		{
			URHO3D_LOGERROR("Failed to change directory to " + pathName);
			return false;
		}
#else
		//todo
#endif
	}

	bool FileSystem::CreateDir(const String &pathName)
	{
		if(!CheckAccess(pathName))
		{
			URHO3D_LOGERROR("Access denied to " + pathName);
			return false;
		}
		String parentPath = GetParentPath(pathName);
		if(parentPath.Length() > 1 && !DirExists(parentPath))
		{
			if(!CreateDir(parentPath))
				return false;
		}
#ifdef _WIN32
		bool success = (CreateDirectoryW(GetWideNativePath(RemoveTrailingSlash(pathName)).CString(), nullptr) == TRUE) ||
				(GetLastError() == ERROR_ALREADY_EXISTS);
#else
		//todo
#endif
		if(success)
			URHO3D_LOGDEBUG("Create directory " + pathName);
		else
			URHO3D_LOGERROR("Failed to create directory " + pathName);
		return success;
	}

	void FileSystem::SetExecuteConsoleCommands(bool enable)
	{
		if(enable == executeConsoleCommands_)
			return;
		executeConsoleCommands_ = enable;
		if(enable)
		{
			SubscribeToEvent(E_CONSOLECOMMAND, URHO3D_HANDLER(FileSystem, HandleConsoleCommand));
		}
		else
		{
			UnsubscribeFromEvent(E_CONSOLECOMMAND);
		}
	}

	int FileSystem::SystemCommand(const String &commandLine, bool redirectStdOutToLog)
	{
		if(allowedPaths_.Empty())
			return DoSystemCommand(commandLine, redirectStdOutToLog, context_);
		else
		{
			URHO3D_LOGERROR("Executing an external command is not allowed");
			return -1;
		}
	}

	int FileSystem::SystemRun(const String &fileName, const Vector<String> &arguments)
	{
		if(allowedPaths_.Empty())
			return DoSystemRun(fileName, arguments);
		else
		{
			URHO3D_LOGERROR("Executing an external command is not allowed");
			return -1;
		}
	}

	unsigned FileSystem::SystemCommandAsync(const String &commandLine)
	{
#ifdef URHO3D_THREADING
		if(allowedPaths_.Empty())
		{
			unsigned requestID = nextAsyncExecID_;
			AsyncSystemCommand* cmd = new AsyncSystemCommand(nextAsyncExecID_, commandLine);
			asyncExecQueue_.Push(cmd);
			return requestID;
		}
		else
		{
			URHO3D_LOGERROR("Executing an external command is not allowed");
			return M_MAX_UNSIGNED;
		}
#else
		URHO3D_LOGERROR("Can not excute an asynchronous command as threading is disabled");
		return M_MAX_UNSIGNED;
#endif
	}

	unsigned FileSystem::SystemRunSync(const String &fileName, const Vector<String> &arguments)
	{
#ifdef URHO3D_THREADING
		if(allowedPaths_.Empty())
		{
			unsigned requestID = nextAsyncExecID_;
			AsyncSystemRun* cmd = new AsyncSystemRun(nextAsyncExecID_, fileName, arguments);
			asyncExecQueue_.Push(cmd);
			return requestID;
		} else {
			URHO3D_LOGERROR("Executing an external command is not allowed");
			return M_MAX_UNSIGNED;
		}
#else
		URHO3D_LOGERROR("Can not run asynchronously as threading is disabled");
		return M_MAX_UNSIGNED;
#endif
	}

	bool FileSystem::SystemOpen(const String &fileName, const String &mode)
	{
		if(allowedPaths_.Empty())
		{
			if(!FileExists(fileName) && !DirExists(fileName))
			{
				URHO3D_LOGERROR("File or directory " + fileName + " not found");
				return false;
			}

#ifdef _WIN32
			bool success = (size_t)ShellExecuteW(nullptr, !mode.Empty() ? WString(mode).CString() : nullptr,
												GetWideNativePath(fileName).CString(), nullptr, nullptr, SW_SHOW) > 32;
#else
			//todo
#endif

		} else {
			URHO3D_LOGERROR("Opening a file externally is not allowed");
			return false;
		}
	}

	bool FileSystem::Copy(const String &srcFileName, const String &destFileName)
	{
		if(!CheckAccess(GetPath(srcFileName)))
		{
			URHO3D_LOGERROR("Access denied to " + srcFileName);
			return false;
		}
		if(!CheckAccess(GetPath(destFileName)))
		{
			URHO3D_LOGERROR("Access denied to " + destFileName);
			return false;
		}

		SharedPtr<File> srcFile(new File(context_, srcFileName, FILE_READ));
		if(!srcFile->IsOpen())
			return false;
		SharedPtr<File> destFile(new File(context_, destFileName, FILE_WRITE));
		if(!destFile->IsOpen())
			return false;
		unsigned fileSize = srcFile->GetSize();
		SharedArrayPtr<unsigned char> buffer(new unsigned char[fileSize]);

		unsigned bytesRead = srcFile->Read(buffer.Get(), fileSize);
		unsigned bytesWritten = destFile->Write(buffer.Get(), fileSize);
		return bytesRead == fileSize && bytesWritten == fileSize;
	}

	bool FileSystem::Rename(const String &srcFileName, const String &destFileName)
	{
		//todo
	}

	bool FileSystem::Delete(const String &fileName)
	{
		return false;
	}

	void FileSystem::RegisterPath(const String &pathName)
	{

	}

	bool FileSystem::SetLastModifiedTime(const String &fileName, unsigned newTime)
	{
		return false;
	}

	String FileSystem::GetCurrentDir() const
	{
		return String();
	}

	bool FileSystem::CheckAccess(const String &pathName) const
	{
		String fixedPath = AddTrailingSlash(pathName);

		if(allowedPaths_.Empty())
			return true;

		if(fixedPath.Contains(".."))
			return false;

		for(auto it = allowedPaths_.Begin(); it != allowedPaths_.End(); ++it)
		{
			//Math at begin
			if(fixedPath.Find(*it) == 0)
				return true;
		}
		//Not found, so disallow
		return false;
	}

	unsigned FileSystem::GetLastModifiedTime(const String &fileName) const
	{
		return 0;
	}

	bool FileSystem::FileExists(const String &fileName) const
	{
		return false;
	}

	bool FileSystem::DirExists(const String &pathName) const
	{
		return false;
	}

	void FileSystem::ScanDir(Vector<String> &result, const String &pathName, const String &filter, unsigned flags,
	                         bool recursive) const
	{

	}

	String FileSystem::GetProgramDir() const
	{
		return String();
	}

	String FileSystem::GetUserDocumentDir() const
	{
		return String();
	}

	String FileSystem::GetAppPreferenceDir(const String &org, const String &app) const
	{
		return String();
	}

	String FileSystem::GetTemporaryDir() const
	{
		return String();
	}

	void FileSystem::ScanDirInternal(Vector<String> &result, String path, const String &startPath, const String &filter,
	                                 unsigned flags, bool recursive) const
	{
		//todo
	}

	void FileSystem::HandleBeginFrame(StringHash eventType, VariantMap &eventData)
	{
		for(auto it = asyncExecQueue_.Begin(); it != asyncExecQueue_.End(); )
		{
			AsyncExecRequest* request = *it;
			if(request->IsCompleted())
			{
				using namespace AsyncExecFinished;
				VariantMap& newEventData = GetEventDataMap();
				newEventData[P_REQUESTID] = request->GetRequestID();
				newEventData[P_EXITCODE] = request->GetExitCode();
				SendEvent(E_ASYNCEXECFINISHED, newEventData);

				delete request;
				it = asyncExecQueue_.Erase(it);
			}
			else
				++it;
		}
	}

	void FileSystem::HandleConsoleCommand(StringHash eventType, VariantMap &eventData)
	{
		using namespace ConsoleCommand;
		if(eventData[P_ID].GetString() == GetTypeName())
			SystemCommand(eventData[P_COMMAND].GetString(), true);
	}

	void
	SplitPath(const String &fullPath, String &pathName, String &fileName, String &extension, bool lowercaseExtension)
	{
		String fullPathCopy = GetInternalPath(fullPath);

		unsigned extPos = fullPathCopy.FindLast('.');
		unsigned pathPos = fullPathCopy.FindLast('/');

		if(extPos != String::NPOS && (pathPos == String::NPOS || extPos > pathPos))
		{
			extension = fullPathCopy.SubString(extPos);
			if(lowercaseExtension)
				extension = extension.ToLower();

			fullPathCopy = fullPathCopy.SubString(0, extPos);
		} else {
			extension.Clear();
		}

		pathPos = fullPathCopy.FindLast('/');
		if(pathPos != String::NPOS)
		{
			fileName = fullPathCopy.SubString(pathPos + 1);
			pathName = fullPathCopy.SubString(0, pathPos + 1);
		} else {
			fileName = fullPathCopy;
			pathName.Clear();
		}
	}

	String GetPath(const String &fullPath)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension);
		return path;
	}

	String GetFileName(const String &fullPath)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension);
		return file;
	}

	String GetExtension(const String &fullPath, bool lowercaseExtension)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension,lowercaseExtension);
		return extension;
	}

	String GetFileNameAndExtension(const String &fullPath, bool lowercaseExtension)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension,lowercaseExtension);
		return file + extension;
	}

	String ReplaceExtension(const String &fullPath, const String &newExtension)
	{
		String path, file, extension;
		SplitPath(fullPath, path, file, extension);
		return path + file + newExtension;
	}

	String AddTrailingSlash(const String &pathName)
	{
		String ret = pathName.Trimmed();
		ret.Replace('\\', '/');
		if(!ret.Empty() && ret.Back() != '/')
			ret += '/';
		return ret;
	}

	String RemoveTrailingSlash(const String &pathName)
	{
		String ret = pathName.Trimmed();
		ret.Replace('\\', '/');
		if(!ret.Empty() && ret.Back() == '/')
			ret.Resize(ret.Length() - 1);
		return ret;
	}

	String GetParentPath(const String &pathName)
	{
		unsigned pos = RemoveTrailingSlash(pathName).FindLast('/');
		if(pos != String::NPOS)
			return pathName.SubString(0, pos + 1);
		return String();
	}

	String GetInternalPath(const String &pathName)
	{
		return pathName.Replaced('\\', '/');
	}

	String GetNativePath(const String &pathName)
	{
#ifdef _WIN32
		return pathName.Replaced('/', '\\');
#else
		return pathName;
#endif
	}

	WString GetWideNativePath(const String &pathName)
	{
#ifdef _WIN32
		return WString(pathName.Replaced('/', '\\'));
#else
		return WString(pathName);
#endif
	}

	bool IsAbsolutePath(const String &pathName)
	{
		if(pathName.Empty())
			return false;
		String path = GetInternalPath(pathName);
		if(path[0] == '/')
			return true;
#ifdef _WIN32
		if(path.Length() > 1 && IsAlpha(path[0]) && path[1] == ':')
			return true;
#endif
		return false;
	}
}
