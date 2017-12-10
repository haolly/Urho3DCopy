//
// Created by liuhao on 2017/12/9.
//

#ifndef URHO3DCOPY_FILESYSTEM_H
#define URHO3DCOPY_FILESYSTEM_H

#include "../Core/Object.h"
#include "../Container/HashSet.h"

namespace Urho3D
{
	class FileSystem : public Object
	{
		URHO3D_OBJECT(FileSystem, Object);
	public:
		FileSystem(Context* context);
		virtual ~FileSystem() override ;

		bool SetCurrentDir(const String& pathName);
		bool CreateDir(const String& pathName);
		void SetExecuteConsoleCommands(bool enable);
		int SystemCommand(const String& commandLine, bool redirectStdOutToLog = false);
		int SystemRun(const String& fileName, const Vector<String>& arguments);
		unsigned SystemCommandAsync(const String& commandLine);
		unsigned SystemRunSync(const String& fileName, const Vector<String>& arguments);
		bool SystemOpen(const String& fileName, const String& mode = String::EMPTY);
		bool Copy(const String& srcFileName, const String& destFileName);
		bool Rename(const String& srcFileName, const String& destFileName);
		bool Delete(const String& fileName);
		void RegisterPath(const String& pathName);
		bool SetLastModifiedTime(const String& fileName, unsigned newTime);

		String GetCurrentDir() const;
		bool GetExecuteConsoleCommands() const
		{
			return executeConsoleCommands_;
		}

		bool HashRegisteredPaths() const
		{
			return allowedPaths_.Size() > 0 ;
		}

		bool CheckAccess(const String& pathName) const;
		unsigned GetLastModifiedTime(const String& fileName) const;
		bool FileExists(const String& fileName) const;
		bool DirExists(const String& pathName) const;
		void ScanDir(Vector<String>& result, const String& pathName, const String& filter,
					unsigned flags, bool recursive) const;
		String GetProgramDir() const;
		String GetUserDocumentDir() const;
		String GetAppPreferenceDir(const String& org, const String& app) const;
		String GetTemporaryDir() const;

	private:
		void ScanDirInternal(Vector<String>& result, String path, const String& startPath,
							const String& filter, unsigned flags, bool recursive) const ;
		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
		void HandleConsoleCommand(StringHash eventType, VariantMap& eventData);

		HashSet<String> allowedPaths_;
		List<AsyncExecRequest*> asyncExecQueue_;
		unsigned nextAsyncExecID_;
		bool executeConsoleCommands_;
	};

	//todo, why define here ?
	String GetPath(const String& fullPath);
}



#endif //URHO3DCOPY_FILESYSTEM_H
