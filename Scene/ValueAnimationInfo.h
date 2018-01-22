//
// Created by liuhao on 2017/12/27.
//

#ifndef URHO3DCOPY_VALUEANIMATIONINFO_H
#define URHO3DCOPY_VALUEANIMATIONINFO_H

#include "../Container/RefCounted.h"
#include "../Core/Variant.h"
#include "../Container/Ptr.h"
#include "../Core/Object.h"
#include "AnimationDefs.h"
#include "ValueAnimation.h"

namespace Urho3D
{
	class ValueAnimationInfo : public RefCounted
	{
	public:
		ValueAnimationInfo(ValueAnimation* animation, WrapMode wrapMode, float speed);
		ValueAnimationInfo(Object* target, ValueAnimation* animation, WrapMode wrapMode, float speed);
		ValueAnimationInfo(const ValueAnimationInfo& other);
		virtual ~ValueAnimationInfo() override ;

		bool Update(float timeStep);
		bool SetTime(float time);

		void SetWrapMode(WrapMode wrapMode)
		{
			wrapMode_ = wrapMode;
		}

		void SetSpeed(float speed)
		{
			speed_ = speed;
		}

		Object* GetTarget() const;
		ValueAnimation* GetAnimation() const { return animation_; }

		WrapMode GetWrapMode() const { return wrapMode_; }
		float GetTime() const { return currentTime_; }
		float GetSpeed() const { return speed_; }

	protected:
		virtual void ApplyValue(const Variant& newValue);
		float CalculateScaledTime(float currentTime, bool& finished) const;
		void GetEventFrames(float beginTime, float endTime, PODVector<const VAnimEventFrame*>& eventFrames);

		//Note, weak reference
		WeakPtr<Object> target_;
		SharedPtr<ValueAnimation> animation_;
		WrapMode wrapMode_;
		float speed_;
		float currentTime_;
		float lastScaledTime_;
	};

}



#endif //URHO3DCOPY_VALUEANIMATIONINFO_H
