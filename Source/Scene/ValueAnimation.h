//
// Created by liuhao on 2017/12/27.
//

#ifndef URHO3DCOPY_VALUEANIMATION_H
#define URHO3DCOPY_VALUEANIMATION_H

#include "../Resource/Resource.h"

namespace Urho3D
{
	enum InterpMethod
	{
		IM_NONE = 0,
		IM_LINEAR,
		IM_SPLINE,
	};

	struct VAnimKeyFrame
	{
		float time_;
		Variant value_;
	};

	struct VAnimEventFrame
	{
		float time_;
		StringHash eventType_;
		VariantMap eventData_;
	};

	class ValueAnimation : public Resource
	{
		URHO3D_OBJECT(ValueAnimation, Resource);
	public:
		ValueAnimation(Context* context);

		virtual ~ValueAnimation() override ;
		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;
		virtual bool Save(Serializer& dest) const override ;
		bool LoadXML(const XMLElement& source);
		bool SaveXML(XMLElement& dest) const;

		void SetOwner(void* owner);
		void SetInterpolationMethod(InterpMethod method);
		void SetSplineTension(float tension);
		void SetValueType(VariantType valueType);

		bool SetKeyFrame(float time, const Variant& value);
		void SetEventFrame(float time, const StringHash& eventType, const VariantMap& eventData = VariantMap());

		bool IsValid() const;

		void* GetOwner() const { return owner_; }

		InterpMethod GetIntepolationMethod() const
		{
			return interpolationMethod_;
		}

		float GetSplineTension() const
		{
			return splineTension_;
		}

		VariantType GetValueType() const
		{
			return valueType_;
		}

		float GetBeginTime() const
		{
			return beginTime_;
		}

		float GetEndTime() const
		{
			return endTime_;
		}

		Variant GetAnimationValue(float scaledTime) const;
		bool HasEventFrames() const
		{
			return !eventFrames_.Empty();
		}

		void GetEventFrames(float beginTime, float endTime, PODVector<const VAnimEventFrame*>& eventFrames) const;

	protected:
		Variant LinearInterpolation(unsigned index1, unsigned index2, float scaledTime) const;
		Variant SplineInterpolation(unsigned index1, unsigned index2, float scaledTime) const;
		void UpdateSplineTangents() const;
		Variant SubstractAndMultiply(const Variant& vlaue1, const Variant& value2, float t) const;

		void* owner_;
		InterpMethod interpolationMethod_;
		float splineTension_;
		VariantType valueType_;
		bool interpolatable_;
		float beginTime_;
		float endTime_;
		Vector<VAnimKeyFrame> keyFrames_;
		mutable VariantVector splineTangents_;
		mutable bool splineTangentsDirty_;
		Vector<VAnimEventFrame> eventFrames_;
	};
}


#endif //URHO3DCOPY_VALUEANIMATION_H
