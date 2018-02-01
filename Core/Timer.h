//
// Created by LIUHAO on 2017/12/7.
//

#ifndef URHO3DCOPY_TIMER_H
#define URHO3DCOPY_TIMER_H

#include "Object.h"

namespace Urho3D
{
	class Timer
	{
	public:
		Timer();
		unsigned GetMSec(bool reset);
		void Reset();
	private:
		unsigned startTime_;
	};

	class HiresTimer
	{
		friend class Time;
	public:
		HiresTimer();
		// Return elapsed microseconds and optionally reset
		long long GetUSec(bool reset);
		void Reset();
		static bool IsSupported() { return supported; }
		static long long GetFrequency() { return frequency; }
	private:
		long long startTime_;
		static bool supported;
		static long long frequency;
	};

	class Time : public Object
	{
		URHO3D_OBJECT(Time, Object);
	public:
		Time(Context* context);
		virtual ~Time() override;

		void BeginFrame(float timeStep);
		void EndFrame();
		void SetTimerPeriod(unsigned mSec);

		unsigned GetFrameNumber() const { return frameNumber_; }
		// todo , usage
		float GetTimeStep() const { return timeStep_; }
		//todo ,usage
		unsigned GetTimerPeroid() const { return timerPeriod_; }
		float GetElapsedTime();
		float GetFramesPerSecond() const;

		static unsigned GetSystemTime();
		static unsigned GetTimeSinceEpoch();
		static String GetTimeStamp();
		static void Sleep(unsigned mSec);


	private:
		Timer elapsedTime_;
		unsigned frameNumber_;
		float timeStep_;
		unsigned timerPeriod_;
	};

}



#endif //URHO3DCOPY_TIMER_H
