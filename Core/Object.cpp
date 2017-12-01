//
// Created by liuhao on 2017/11/27.
//

#include "Object.h"

namespace Urho3D
{
    TypeInfo::TypeInfo(const char *typeName, const TypeInfo *baseTypeInfo) :
        type_(typeName),
        typeName_(typeName),
        baseTypeInfo_(baseTypeInfo)
    {
    }

    TypeInfo::~TypeInfo()
    {

    }

    bool TypeInfo::IsTypeOf(StringHash type) const
    {
        const TypeInfo* current = this;
        while (current)
        {
            if(current->GetType() == type)
            {
                return true;
            }
            current = current->GetBaseTypeInfo();
        }
        return false;
    }

    bool TypeInfo::IsTypeOf(const TypeInfo *typeInfo) const
    {
        const TypeInfo* current = this;
        while (current)
        {
            if(current == typeInfo)
            {
                return true;
            }
            current = current->GetBaseTypeInfo();
        }
        return false;
    }

    Object::Object(Context *context) :
		context_(context),
		blockEvents_(false)
    {
    }

	Object::~Object()
	{
		UnsubscribeFromAllEvents();
		context_->RemoveEventSender(this);
	}
}

