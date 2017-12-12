//
// Created by LIUHAO on 2017/12/8.
//

#ifndef URHO3DCOPY_LOG_H
#define URHO3DCOPY_LOG_H

#include "../Container/Str.h"
#include "../Core/Object.h"
#include "../Container/List.h"

namespace Urho3D
{
	static const int LOG_RAW = -1;
	static const int LOG_TRACE = 0;
	static const int LOG_DEBUG = 1;
	static const int LOG_INFO = 2;
	static const int LOG_WARNING = 3;
	static const int LOG_ERROR = 4;
	static const int LOG_NONE = 5;

	class File;

	struct StoredLogMessage
	{
		StoredLogMessage()
		{
		}

		StoredLogMessage(const String& message, int level, bool error) :
			message_(message),
			level_(level),
			error_(error)
		{
		}

		String message_;
		int level_;
		bool error_;
	};

	class Log : public Object
	{
		URHO3D_OBJECT(Log, Object);
	public:
		Log(Context* context);
		virtual ~Log() override ;

		void Open(const String& fileName);
		void Close();
		void SetLevel(int level);
		void SetTimeStamp(bool enable);
		void SetQuiet(bool quiet);

		int GetLevel() const { return level_; }
		bool GetTimeStamp() const { return timeStamp_; }
		String GetLastMessage() const { return lastMessage_; }
		bool IsQuiet() const { return quiet_; }

		static void Write(int level, const String& message);
		static void WriteRaw(const String& message, bool error = false);

	private:
		void HandleEndFrame(StringHash eventType, VariantMap& eventData);
		Mutex logMutex_;
		List<StoredLogMessage> threadMessages_;
		SharedPtr<File> logFile_;
		String lastMessage_;
		int level_;
		bool timeStamp_;
		bool inWrite_;
		bool quiet_;
	};

#define URHO3D_LOGTRACE(message) Urho3D::Log::Write(Urho3D::LOG_TRACE, message)
#define URHO3D_LOGDEBUG(message) Urho3D::Log::Write(Urho3D::LOG_DEBUG, message)
#define URHO3D_LOGINFO(message) Urho3D::Log::Write(Urho3D::LOG_INFO, message)
#define URHO3D_LOGWARNING(message) Urho3D::Log::Write(Urho3D::LOG_WARNING, message)
#define URHO3D_LOGERROR(message) Urho3D::Log::Write(Urho3D::LOG_ERROR, message)
#define URHO3D_LOGRAW(message) Urho3D::Log::WriteRaw(message)
#define URHO3D_LOGTRACEF(format, ...) Urho3D::Log::Write(Urho3D::LOG_TRACE, Urho3D::ToString(format, ##__VA_ARGS__))
#define URHO3D_LOGDEBUGF(format, ...) Urho3D::Log::Write(Urho3D::LOG_DEBUG, Urho3D::ToString(format, ##__VA_ARGS__))
#define URHO3D_LOGINFOF(format, ...) Urho3D::Log::Write(Urho3D::LOG_INFO, Urho3D::ToString(format, ##__VA_ARGS__))
#define URHO3D_LOGWARNINGF(format, ...) Urho3D::Log::Write(Urho3D::LOG_WARNING, Urho3D::ToString(format, ##__VA_ARGS__))
#define URHO3D_LOGERRORF(format, ...) Urho3D::Log::Write(Urho3D::LOG_ERROR, Urho3D::ToString(format, ##__VA_ARGS__))
#define URHO3D_LOGRAWF(format, ...) Urho3D::Log::WriteRaw(Urho3D::ToString(format, ##__VA_ARGS__))

}


#endif //URHO3DCOPY_LOG_H
