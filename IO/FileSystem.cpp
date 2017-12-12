//
// Created by liuhao on 2017/12/9.
//

#include <processthreadsapi.h>
#include <winbase.h>
#include "FileSystem.h"
#include "../Core/Thread.h"
#include "../Math/MathDefs.h"
#include "../ThirdParty/SDL/include/SDL_stdinc.h"
#include "../ThirdParty/SDL/include/SDL_filesystem.h"
#include "File.h"
#include "../Core/CoreEvent.h"
#include "../Engine/EngineEvents.h"


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
				//todo log
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
			//todo  log error ("Access denied to " + pathName)
			return false;
		}
#ifdef _WIN32
		if(SetCurrentDirectoryW(GetWideNativePath(pathName).CString()) == FALSE)
		{
			//todo log error (Failed to change directory to " pathName
			return false;
		}
#else
		//todo
#endif
	}

	bool FileSystem::CreateDir(const String &pathName)
	{
		return false;
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
		return 0;
	}

	int FileSystem::SystemRun(const String &fileName, const Vector<String> &arguments)
	{
		return 0;
	}

	unsigned FileSystem::SystemCommandAsync(const String &commandLine)
	{
		return 0;
	}

	unsigned FileSystem::SystemRunSync(const String &fileName, const Vector<String> &arguments)
	{
		return 0;
	}

	bool FileSystem::SystemOpen(const String &fileName, const String &mode)
	{
		return false;
	}

	bool FileSystem::Copy(const String &srcFileName, const String &destFileName)
	{
		return false;
	}

	bool FileSystem::Rename(const String &srcFileName, const String &destFileName)
	{
		return false;
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

	}

	void FileSystem::HandleBeginFrame(StringHash eventType, VariantMap &eventData)
	{

	}

	void FileSystem::HandleConsoleCommand(StringHash eventType, VariantMap &eventData)
	{

	}
}
