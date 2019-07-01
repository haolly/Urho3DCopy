//
// Created by LIUHAO on 2017/12/5.
//

#ifndef URHO3DCOPY_ATTRIBUTE_H
#define URHO3DCOPY_ATTRIBUTE_H

#include "../Container/RefCounted.h"
#include "Variant.h"
#include "../Container/Ptr.h"

namespace Urho3D
{
	static const unsigned AM_EDIT = 0x0;
	// Attribute used for file serialization
	static const unsigned AM_FILE = 0x1;
	static const unsigned AM_NET = 0x2;
	static const unsigned AM_DEFAULT = 0x3;
	// Attribute should use latest data grouping instead of delta update in network replication
	static const unsigned AM_LATESTDATA = 0x4;
	// Attribute should not be shown in the editor
	static const unsigned AM_NOEDIT = 0x8;
	// Attribute is a node ID and may need rewriting
	static const unsigned AM_NODEID = 0x10;
	static const unsigned AM_COMPONENTID = 0x20;
	// Attribhute is a node ID vector where first element is the amount of modes
	static const unsigned AM_NODEIDVECTOR = 0x40;
	static const unsigned AM_FILEREADONLY = 0x81;


	class Serializable;

	class AttributeAccessor : public RefCounted
	{
	public:
		virtual void Get(const Serializable* ptr, Variant& dest) const = 0;
		virtual void Set(Serializable* ptr, const Variant& src) = 0;
	};

	struct AttributeInfo
	{
		AttributeInfo()
		{
		}

		AttributeInfo(VariantType type, const char* name, SharedPtr<AttributeAccessor> access,
						const char** enumNames, const Variant& defaultValue, unsigned mode):
					type_(type),
					name_(name),
					enumNames_(enumNames),
					accessor_(access),
					defaultValue_(defaultValue),
					mode_(mode)
		{
		}

		const Variant& GetMetadata(const StringHash& key) const
		{
			auto elem = metadata_.Find(key);
			return elem != metadata_.End() ? elem->second_ : Variant::EMPTY;
		}

		template <class T>
		T GetMetadata(const StringHash& key) const
		{
			return GetMetadata(key).Get<T>();
		}

		VariantType type_ = VAR_NONE;
		String name_;
		//Todo, all possibility enum name ?
		const char** enumNames_ = nullptr;
		SharedPtr<AttributeAccessor> accessor_;
		Variant defaultValue_;
		// todo, 啥时候设置的？ 例如 Scene 文件， ref: Scene:PreloadResources
		unsigned mode_ = AM_DEFAULT;
		VariantMap metadata_;
		void* ptr_ = nullptr;
	};

	struct AttributeHandle
	{
		friend class Context;

	private:
		AttributeHandle() = default;
		AttributeHandle(const AttributeHandle& another) = default;
		AttributeInfo* attributeInfo_ = nullptr;
		AttributeInfo* networkAttributeInfo_ = nullptr;
	public:
		AttributeHandle& SetMetadata(StringHash key, const Variant& value)
		{
			if(attributeInfo_)
				attributeInfo_->metadata_[key] = value;
			if(networkAttributeInfo_)
				networkAttributeInfo_->metadata_[key] = value;
			return *this;
		}
	};
}

#endif //URHO3DCOPY_ATTRIBUTE_H
