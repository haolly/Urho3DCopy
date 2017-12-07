//
// Created by LIUHAO on 2017/12/7.
//

#include "Timer.h"
#include "CoreEvent.h"
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

namespace Urho3D
{
	bool HiresTimer::supported(false);
	long long HiresTimer::frequency(1000);

	Time::Time(Context *context) :
			Object(context),
			frameNumber_(0),
			timeStep_(0.0f),
			timerPeriod_(0)
	{
#ifdef _WIN32
		LARGE_INTEGER frequency;
		if(QueryPerformanceFrequency(&frequency))
		{
			HiresTimer::frequency = frequency.QuadPart;
			HiresTimer::supported = true;
		}
#else
		HiresTimer::frequency = 1000000;
		HiresTimer::supported = true;
#endif
	}

	Time::~Time()
	{
		SetTimerPeriod(0);
	}

	static unsigned Tick()
	{
#ifdef _WIN32
		return (unsigned)timeGetTime();
#else
		struct timeval time;
		gettimeofday(&time, NULL);
		return (unsigned)(time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
	}

	static long long HiresTick()
	{
#ifdef _WIN32
		if(HiresTimer::IsSupported())
		{
			LARGE_INTEGER counter;
			QueryPerformanceCounter(&counter);
			return counter.QuadPart;
		} else {
			return timeGetTime();
		}
#else
		struct timeval time;
		gettimeofday(&time, NULL);
		return time.tv_sec * 1000000LL + time.tv_usec;
#endif
	}

	void Time::BeginFrame(float timeStep)
	{
		++frameNumber_;
		if(!frameNumber_)
			++frameNumber_;

		timeStep_  = timeStep;
		//todo profiler
		{
			using namespace BeginFrame;
			VariantMap& eventData = GetEventDataMap();
			eventData[P_FRAMENUMBER] = frameNumber_;
			eventData[P_TIMESTEP] = timeStep_;
			SendEvent(E_BEGINFRAME, eventData);
		}
	}

	void Time::EndFrame()
	{
		//todo profiler
		SendEvent(E_ENDFRAME);
	}


	float Time::GetElapsedTime()
	{
		return elapsedTime_.GetMSec(false) / 1000.0f;
	}

	float Time::GetFramesPerSecond() const
	{
		return 1.0f / timeStep_;
	}

	unsigned Time::GetSystemTime()
	{
		return Tick();
	}

	unsigned Time::GetTimeSinceEpoch()
	{
		return (unsigned)time(nullptr);
	}

	String Time::GetTimeStamp()
	{
		time_t sysTime;
		time(&sysTime);
		const char* dataTime = ctime(&sysTime);
		return String(dataTime).Replaced("\n", "");
	}

	void Time::Sleep(unsigned mSec)
	{
#ifdef _WIN32
		::Sleep(mSec);
#else
		timespec time;
		time.tv_sec = mSec / 1000;
		time.tv_nsec = (mSec % 1000) * 1000000;
		nanosleep(&time, 0);
#endif
	}

	unsigned Timer::GetMSec(bool reset)
	{
		unsigned currentTime = Tick();
		unsigned elapsedTime = currentTime - startTime_;
		if (reset)
			startTime_ = currentTime;

		return elapsedTime;
	}

	Timer::Timer()
	{
		Reset();
	}

	void Timer::Reset()
	{
		startTime_ = Tick();
	}


	HiresTimer::HiresTimer()
	{
		Reset();
	}

	void HiresTimer::Reset()
	{
		startTime_ = HiresTick();
	}

	long long HiresTimer::GetUSec(bool reset)
	{
		long long currentTime = HiresTick();
		long long elapsedTime = currentTime - startTime_;

		if(elapsedTime < 0)
			elapsedTime = 0;

		if(reset)
			startTime_ = currentTime;

		return (elapsedTime * 1000000LL) / frequency;
	}
}


