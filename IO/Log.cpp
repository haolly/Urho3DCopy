//
// Created by LIUHAO on 2017/12/8.
//

#include "Log.h"
#include "../Core/CoreEvent.h"
#include "../IO/File.h"
#include "../Core/StringUtils.h"
#include "../Core/Thread.h"
#include "../Core/Timer.h"
#include "IOEvent.h"

namespace Urho3D
{
	const char* logLevelPrefixes[] =
	{
		"TRACE",
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR",
		nullptr
	};

	static Log* logInstance = nullptr;
	static bool threadErrorDisplayed = false;

	Log::Log(Context *context) :
		Object(context),
#if _DEBUG
		level_(LOG_DEBUG),
#else
		level_(LOG_INFO),
#endif
		timeStamp_(true),
		inWrite_(false),
		quiet_(false)
	{
		logInstance = this;
		SubscribeToEvent(E_ENDFRAME, URHO3D_HANDLER(Log, HandleEndFrame));
	}

	Log::~Log()
	{
		logInstance = nullptr;
	}

	void Log::Open(const String &fileName)
	{
#if !defined(__ANDROID__) && !defined(IOS) && !defined(TVOS)
		if(fileName.Empty())
			return;
		if(logFile_ && logFile_->IsOpen())
		{
			if(logFile_->GetName() == fileName)
				return;
			else
				Close();
		}

		logFile_ = new File(context_);
		if(logFile_->Open(fileName, FILE_WRITE))
			Write(LOG_INFO, "Opened log file " + fileName);
		else
		{
			logFile_.Reset();
			Write(LOG_ERROR, "Failed to create log file " + fileName);
		}
#endif
	}

	void Log::Close()
	{
#if !defined(__ANDROID__) && !defined(IOS) && !defined(TVOS)
		if(logFile_ && logFile_->IsOpen())
		{
			logFile_->Close();
			logFile_.Reset();
		}
#endif
	}

	void Log::SetLevel(int level)
	{
		if(level < LOG_TRACE || level > LOG_NONE)
		{
			URHO3D_LOGERRORF("Attempted to set erroneous log level %d", level);
			return;
		}
		level_ = level;
	}

	void Log::SetTimeStamp(bool enable)
	{
		timeStamp_ = enable;
	}

	void Log::SetQuiet(bool quiet)
	{
		quiet_ = quiet;
	}

	void Log::Write(int level, const String &message)
	{
		if(level == LOG_RAW)
		{
			WriteRaw(message, false);
			return;
		}
		if(level < LOG_TRACE || level > LOG_NONE)
			return;

		// If not in the main thread, store message for later processing
		if(!Thread::IsMainThread())
		{
			if(logInstance)
			{
				MutexLock lock(logInstance->logMutex_);
				logInstance->threadMessages_.Push(StoredLogMessage(message, level, false));
			}
			return;
		}

		if(!logInstance || logInstance->level_ > level || logInstance->inWrite_)
			return;

		String formattedMessage = logLevelPrefixes[level];
		formattedMessage += ": " + message;
		logInstance->lastMessage_ = message;
		if(logInstance->timeStamp_)
			formattedMessage = "[" + Time::GetTimeStamp() + "] " + formattedMessage;

#if defined(__ANDROID__)
		//todo
#elif defined(IOS) || defined(TVOS)
		//todo
#else
		if(logInstance->quiet_)
		{
			if(level == LOG_ERROR)
				PrintUnicodeLine(formattedMessage, true);
		}
		else
			PrintUnicodeLine(formattedMessage, level == LOG_ERROR);
#endif
		if(logInstance->logFile_)
		{
			logInstance->logFile_->WriteLine(formattedMessage);
			logInstance->logFile_->Flush();
		}

		logInstance->inWrite_ = true;
		using namespace LogMessage;
		VariantMap& eventData = logInstance->GetEventDataMap();
		eventData[P_MESSAGE] = formattedMessage;
		eventData[P_LEVEL] = level;
		logInstance->SendEvent(E_LOGMESSAGE, eventData);
		logInstance->inWrite_ = false;
	}

	void Log::WriteRaw(const String &message, bool error)
	{

	}

	void Log::HandleEndFrame(StringHash eventType, VariantMap &eventData)
	{

	}
}
