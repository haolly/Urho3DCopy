//
// Created by liuhao1 on 2018/1/25.
//

#ifndef URHO3DCOPY_UNKNOWNCOMPONENT_H
#define URHO3DCOPY_UNKNOWNCOMPONENT_H

#include "Component.h"

namespace Urho3D
{
	class UnknownComponent : public Component
	{
	public:
		UnknownComponent(Context* context);
		static void RegisterObject(Context* context);

		virtual StringHash GetType() const override { return typeHash_;}
		virtual const String& GetTypeName() const override  { return typeName_; }
		virtual const Vector<AttributeInfo>* GetAttributes() const override  { return return &xmlAttributeInfos_; }
		virtual bool Load(Deserializer& source, bool setInstanceDefault = false) override ;
		//todo

	private:
		StringHash typeHash_;
		String typeName_;
		Vector<AttributeInfo> xmlAttributeInfos_;
		Vector<String> xmlAttributes_;
		PODVector<unsigned char> binaryAttributes_;
		bool useXML_;
	};

}


#endif //URHO3DCOPY_UNKNOWNCOMPONENT_H
