//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_OBJECT_H
#define URHO3DCOPY_OBJECT_H

#include "../Container/RefCounted.h"
#include "../Math/StringHash.h"
#include "../Container/Str.h"

namespace Urho3D
{
    class Context;
    class EventHandler;

    class TypeInfo
    {
    public:
        TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo);
        ~TypeInfo();

        bool IsTypeOf(StringHash type) const;
        bool IsTypeOf(const TypeInfo* typeInfo) const;

        template <typename T> bool IsTypeOf() const
        {
            IsTypeOf(T::GetTypeInfoStatic());
        }

        StringHash GetType() const
        {
            return type_;
        }

        const String& GetTypeName() const
        {
            return typeName_;
        }

        const TypeInfo* GetBaseTypeInfo() const
        {
            return baseTypeInfo_;
        }

    private:
        StringHash type_;
        String typeName_;
        const TypeInfo* baseTypeInfo_;
    };

    class Object : public RefCounted
    {
        friend class Context;
    };
}


#endif //URHO3DCOPY_OBJECT_H
