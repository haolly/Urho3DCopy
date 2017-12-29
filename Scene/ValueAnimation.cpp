//
// Created by liuhao on 2017/12/27.
//

#include "ValueAnimation.h"
#include "../Math/MathDefs.h"
#include "../Core/Context.h"
#include "../Resource/XMLFile.h"

namespace Urho3D
{
	const char* interpMethodNames[] =
			{
					"None",
					"Linear",
					"Spline",
					nullptr
			};

	ValueAnimation::ValueAnimation(Context *context) :
		Resource(context),
		owner_(nullptr),
		interpolationMethod_(IM_LINEAR),
		splineTension_(0.5f),
		valueType_(VAR_NONE),
		interpolatable_(false),
		beginTime_(M_INFINITY),
		endTime_(-M_INFINITY),
		splineTangentsDirty_(false)
	{
	}

	ValueAnimation::~ValueAnimation()
	{

	}

	void ValueAnimation::RegisterObject(Context *context)
	{
		context->RegisterFactory<ValueAnimation>();
	}

	bool ValueAnimation::BeginLoad(Deserializer &source)
	{
		XMLFile xmlFile(context_);
		if(xmlFile.Load(source))
			return false;
		return LoadXML(xmlFile.GetRoot());
	}

	bool ValueAnimation::Save(Serializer &dest) const
	{
		XMLFile xmlFile(context_);
		XMLElement rootElem = xmlFile.CreateRoot("valueanimation");
		if(!SaveXML(rootElem))
			return false;
		return xmlFile.Save(dest);
	}

	bool ValueAnimation::LoadXML(const XMLElement &source)
	{
		valueType_ = VAR_NONE;
		eventFrames_.Clear();

		String interpMethodString = source.GetAttribute("interpolationmethod");
		InterpMethod method = (InterpMethod)GetStringListIndex(interpMethodString.CString(),
								interpMethodNames, IM_LINEAR);
		SetInterpolationMethod(method);
		if(interpolationMethod_ == IM_SPLINE)
			splineTension_ = source.GetFloat("splinetension");
		XMLElement keyFrameElem = source.GetChild("keyframe");
		while(keyFrameElem)
		{
			float time = keyFrameElem.GetFloat("time");
			Variant value = keyFrameElem.GetVariant();
			SetKeyFrame(time, value);

			keyFrameElem = keyFrameElem.GetNext("keyframe");
		}

		XMLElement eventFrameElem = source.GetChild("eventframe");
		while (eventFrameElem)
		{
			float time = eventFrameElem.GetFloat("time");
			unsigned eventType = eventFrameElem.GetUInt("eventtype");
			VariantMap eventData = eventFrameElem.GetChild("eventdata").GetVariantMap();

			SetEventFrame(time, StringHash(eventType), eventData);
			eventFrameElem = eventFrameElem.GetNext("eventframe");
		}
		return true;
	}

	bool ValueAnimation::SaveXML(XMLElement &dest) const
	{
		dest.SetAttribute("interpolationmethod", interpMethodNames[interpolationMethod_]);
		if(interpolationMethod_ == IM_SPLINE)
			dest.SetFloat("splinetension", splineTension_);
		for(unsigned i=0; i < keyFrames_.Size(); ++i)
		{
			const VAnimKeyFrame& keyFrame = keyFrames_[i];
			XMLElement keyFrameElem = dest.CreateChild("keyframe");
			keyFrameElem.SetFloat("time", keyFrame.time_);
			keyFrameElem.SetVariant(keyFrame.value_);
		}
		for(unsigned i=0; i<eventFrames_.Size(); ++i)
		{
			const VAnimEventFrame& eventFrame = eventFrames_[i];
			XMLElement eventFrameElem = dest.CreateChild("eventframe");
			eventFrameElem.SetFloat("time", eventFrame.time_);
			eventFrameElem.SetUInt("eventtype", eventFrame.eventType_.Value());
			eventFrameElem.CreateChild("eventdata").SetVariantMap(eventFrame.eventData_);
		}
		return true;
	}

	void ValueAnimation::SetOwner(void *owner)
	{
		owner_ = owner;
	}

	void ValueAnimation::SetInterpolationMethod(InterpMethod method)
	{
		if(method == interpolationMethod_)
			return;
		if(method == IM_SPLINE && (valueType_ == VAR_INTRECT || valueType_ == VAR_INTVECTOR3 ||
		                          valueType_ == VAR_INTVECTOR2))
		{
			method = IM_LINEAR;
		}
		interpolationMethod_ = method;
		splineTangentsDirty_ = true;
	}

	void ValueAnimation::SetSplineTension(float tension)
	{
		splineTension_ = tension;
		splineTangentsDirty_ = true;
	}

	void ValueAnimation::SetValueType(VariantType valueType)
	{
		if(valueType == valueType_)
			return;
		valueType_ = valueType;
		interpolatable_ = (valueType_ == VAR_FLOAT) || (valueType_ == VAR_VECTOR2) || (valueType_ == VAR_VECTOR3)
				|| (valueType_ == VAR_VECTOR4) || (valueType_ == VAR_QUATERNION) || (valueType_ == VAR_COLOR);

		if((valueType_ == VAR_INTRECT) || (valueType_ == VAR_INTVECTOR2) || (valueType_ == VAR_INTVECTOR3))
		{
			interpolatable_ = true;
			if(interpolationMethod_ == IM_SPLINE)
				interpolationMethod_ = IM_LINEAR;
		}
		keyFrames_.Clear();
		eventFrames_.Clear();
		beginTime_ = M_INFINITY;
		endTime_ = -M_INFINITY;
	}

	bool ValueAnimation::SetKeyFrame(float time, const Variant &value)
	{
		if (valueType_ == VAR_NONE)
			SetValueType(value.GetType());
		else if(value.GetType() != valueType_)
			return false;

		VAnimKeyFrame keyFrame;
		keyFrame.time_ = time;
		keyFrame.value_ = value;

		if(keyFrames_.Empty() || time > keyFrames_.Back().time_)
			keyFrames_.Push(keyFrame);
		else
		{
			for(unsigned i=0; i<keyFrames_.Size(); ++i)
			{
				if(time == keyFrames_[i].time_)
					return false;
				if(time < keyFrames_[i].time_)
				{
					keyFrames_.Insert(i, keyFrame);
					break;
				}
			}
		}

		beginTime_ = Min(time, beginTime_);
		endTime_ = Max(time, endTime_);
		splineTangentsDirty_ = true;
		return true;
	}

	void ValueAnimation::SetEventFrame(float time, const StringHash &eventType, const VariantMap &eventData)
	{
		VAnimEventFrame eventFrame;
		eventFrame.time_ = time;
		eventFrame.eventType_ = eventType;
		eventFrame.eventData_ = eventData;

		if(eventFrames_.Empty() || time >= eventFrames_.Back().time_)
			eventFrames_.Push(eventFrame);
		else
		{
			for(unsigned i=0; i<eventFrames_.Size(); ++i)
			{
				if(time < eventFrames_[i].time_)
				{
					eventFrames_.Insert(i, eventFrame);
					break;
				}
			}
		}

		beginTime_ = Min(time, beginTime_);
		endTime_ = Max(time, endTime_);
	}

	bool ValueAnimation::IsValid() const
	{
		return (interpolationMethod_ == IM_NONE ||
				(interpolationMethod_ == IM_LINEAR && keyFrames_.Size() > 1) ||
				(interpolationMethod_ == IM_SPLINE && keyFrames_.Size() > 2));
	}

	Variant ValueAnimation::GetAnimationValue(float scaledTime) const
	{
		unsigned index = 1;
		for(; index< keyFrames_.Size(); ++index)
		{
			if(scaledTime < keyFrames_[index].time_)
				break;
		}
		if(index >= keyFrames_.Size() || !interpolatable_ || interpolationMethod_ == IM_NONE)
			return keyFrames_[index -1].value_;
		else
		{
			if(interpolationMethod_ == IM_LINEAR)
				return LinearInterpolation(index -1, index, scaledTime);
			else
				return SplineInterpolation(index -1, index, scaledTime);
		}
	}

	void ValueAnimation::GetEventFrames(float beginTime, float endTime,
	                                    PODVector<const VAnimEventFrame *> &eventFrames) const
	{
		for(unsigned i=0; i<eventFrames_.Size(); ++i)
		{
			const VAnimEventFrame& eventFrame = eventFrames_[i];
			if(eventFrame.time_ > endTime)
				break;
			if(eventFrame.time_ >= beginTime)
				eventFrames.Push(&eventFrame);
		}
	}

	Variant ValueAnimation::LinearInterpolation(unsigned index1, unsigned index2, float scaledTime) const
	{
		const VAnimKeyFrame& keyFrame1 = keyFrames_[index1];
		const VAnimKeyFrame& keyFrame2 = keyFrames_[index2];
		float t = (scaledTime - keyFrame1.time_) / (keyFrame2.time_ - keyFrame1.time_);
		const Variant& value1 = keyFrame1.value_;
		const Variant& value2 = keyFrame2.value_;

		switch (valueType_)
		{
			case VAR_FLOAT:
				return Lerp(value1.GetFloat(), value2.GetFloat(), t);
			case VAR_VECTOR2:
				return value1.GetVector2().Lerp(value2.GetVector2(), t);
			//todo
		}
	}

	Variant ValueAnimation::SplineInterpolation(unsigned index1, unsigned index2, float scaledTime) const
	{
		//todo
	}

	void ValueAnimation::UpdateSplineTangents() const
	{
		//todo
	}

	Variant ValueAnimation::SubstractAndMultiply(const Variant &vlaue1, const Variant &value2, float t) const
	{
		//todo
	}
}