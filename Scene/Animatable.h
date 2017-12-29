//
// Created by liuhao on 2017/12/19.
//

#ifndef URHO3DCOPY_ANIMATABLE_H
#define URHO3DCOPY_ANIMATABLE_H

#include "Serializable.h"

namespace Urho3D
{

	class AttributeAnimationInfo : public ValueAnimationInfo
	{

	};

	class Animatable : public Serializable
	{
		URHO3D_OBJECT(Animatable, Serializable);
	public:
		Animatable(Context* context);
		virtual ~Animatable() override ;
		static void RegisterObject(Context* context);

		virtual bool LoadXML(const XMLElement& source, bool setInstanceDefault = false) override ;
		virtual bool SaveXML(XMLElement& dest) const override ;

		void SetAnimationEnabled(bool enable);
		void SetAnimationTime(float time);

		void SetObjectAnimation(ObjectAnimation* objectAnimation);
		//todo
	};
}



#endif //URHO3DCOPY_ANIMATABLE_H
