//
// Created by liuhao on 2017/12/20.
//

#include "Serializable.h"
#include "../IO/Log.h"

namespace Urho3D
{
	static unsigned RemapAttributeIndex(const Vector<AttributeInfo>* attributes,
	                                    const AttributeInfo& netAttr, unsigned netAttriIndex)
	{
		if(!attributes)
			return netAttriIndex;

		for(unsigned i = 0; i< attributes->Size(); ++i)
		{
			const AttributeInfo& attr = attributes->At(i);
			if(attr.accessor_.Get() && attr.accessor_.Get() == netAttr.accessor_.Get())
				return i;
		}

		return netAttriIndex;
	}


	Serializable::Serializable(Context *context) :
			Object(context),
			temporary_(false)
	{

	}

	Serializable::~Serializable()
	{

	}

	//todo, usage
	void Serializable::OnSetAttribute(const AttributeInfo &attr, const Variant &src)
	{
		if(attr.accessor_)
		{
			attr.accessor_->Set(this, src);
			return;
		}

		assert(attr.ptr_);
		void* dest = attr.ptr_;

		switch (attr.type_)
		{
			case VAR_INT:
				//Note, if enum type, use the low 8 bits only
				//todo, usage
				if(attr.enumNames_)
					*(reinterpret_cast<unsigned char*>(dest)) = src.GetInt();
				else
					*(reinterpret_cast<int*>(dest)) = src.GetInt();
				break;
			case VAR_INT64:
				*(reinterpret_cast<long long*>(dest)) = src.GetInt64();
				break;
			case VAR_BOOL:
				*(reinterpret_cast<bool*>(dest)) = src.GetBool();
				break;
			case VAR_FLOAT:
				*(reinterpret_cast<float *>(dest)) = src.GetFloat();
				break;
				//todo
			default:
				URHO3D_LOGERROR("Unsupported attribute type for OnSetAttribute()");
				return;
		}

		if(attr.mode_ & AM_NET)
			MarkNetworkUpdate();
	}

	void Serializable::OnGetAttribute(const AttributeInfo &attr, Variant &dest) const
	{
		if(attr.accessor_)
		{
			attr.accessor_->Get(this, dest);
			return;
		}

		assert(attr.ptr_);
		const void* src = attr.ptr_;

		switch(attr.type_)
		{
			case VAR_INT:
				if(attr.enumNames_)
					dest = *(reinterpret_cast<const unsigned char*>(src));
				else
					dest = *(reinterpret_cast<int*>(src));
				break;
			case VAR_INT64:
				//todo
		}
	}

	const Vector<AttributeInfo> *Serializable::GetAttributes() const
	{
		//todo
	}

	const Vector<AttributeInfo> *Serializable::GetNetworkAttributes() const
	{
		return nullptr;
	}

	bool Serializable::Load(Deserializer &source, bool setInstanceDefault)
	{
		const Vector<AttributeInfo>* attributes = GetAttributes();
		if(!attributes)
			return true;
		for(unsigned i =0; i< attributes->Size(); ++i)
		{
			const AttributeInfo& attr = attributes->At(i);
			if(!(attr.mode_ & AM_FILE))
				continue;

			if(source.IsEof())
			{
				URHO3D_LOGERROR("Could not load " + GetTypeName() + ", stream not open or at end");
				return false;
			}

			Variant varValue = source.ReadVariant(attr.type_);
			OnSetAttribute(attr, varValue);
			if(setInstanceDefault)
				SetInstanceDefault(attr.name_, varValue);
		}
		return true;
	}

	bool Serializable::Save(Serializer &dest) const
	{
		return false;
	}

	bool Serializable::LoadXML(const XMLElement &source, bool setInstanceDefault)
	{
		return false;
	}

	bool Serializable::SaveXML(XMLElement &dest) const
	{
		return false;
	}

	bool Serializable::SetAttribute(unsigned index, const Variant &value)
	{
		const Vector<AttributeInfo>* attributes = GetAttributes();
		if(!attributes)
		{
			URHO3D_LOGERROR(GetTypeName() + " has no attributes");
			return false;
		}
		if(index >= attributes->Size())
		{
			URHO3D_LOGERROR("Attribute index out of bounds");
			return false;
		}

		const AttributeInfo& attr = attributes->At(index);

		if(value.GetType() == attr.type_)
		{
			OnSetAttribute(attr, value);
			return true;
		}
		else
		{
			URHO3D_LOGERROR("Could not set attribute " + attr.name_ + ": expected type" +
							Variant::GetTypeName(attr.type_) + " but got " + value.GetTypeName());

			return false;
		}
	}

	bool Serializable::SetAttribute(const String &name, const Variant &value)
	{
		const Vector<AttributeInfo>* attributes = GetAttributes();
		if(!attributes)
		{
			URHO3D_LOGERROR(GetTypeName() + "has no attributes");
			return false;
		}

		for(auto it = attributes->Begin(); it != attributes->End(); ++it)
		{
			if(!it->name_.Compare(name, true))
			{
				if(value.GetType() == it->type_)
				{
					OnSetAttribute(*it, value);
					return true;
				}
				else
				{
					URHO3D_LOGERROR("Could not set attribute " + it->name_ + " : expected type " +
									Variant::GetTypeName(it->type_) + " but got" + value.GetTypeName());
					return false;
				}
			}
		}

		URHO3D_LOGERROR("Could not find attribute " + name + " in " + GetTypeName());
		return false;
	}

	void Serializable::ResetToDefault()
	{
		const Vector<AttributeInfo>* attributes = GetAttributes();
		if(!attributes)
			return;

		for(unsigned i=0; i<attributes->Size(); ++i)
		{
			const AttributeInfo& attr = attributes->At(i);
			if(attr.mode_ & (AM_NOEDIT | AM_NODEIT | AM_COMPONENTID | AM_NODEIDVECTOR))
				continue;

			Variant defaultValue = GetInstanceDefault(attr.name_);
			if(defaultValue.IsEmpty())
				defaultValue = attr.defaultValue_;
			OnSetAttribute(attr, defaultValue);
		}
	}

	void Serializable::RemoveInstanceDefault()
	{

	}

	void Serializable::SetTemporary(bool enable)
	{

	}

	void Serializable::SetInterceptNewwrokUpdate(const String &attributeName, bool enable)
	{

	}

	void Serializable::AllocateNetworkState()
	{

	}

	void
	Serializable::WriteInitialDeltaUpdate(Serializer &dest, const DirtyBits &attributeBits, unsigned char timeStemp)
	{

	}

	void Serializable::WriteLatestDataUpdate(Serializer &dest, unsigned char timeStamp)
	{

	}

	void Serializable::SetInstanceDefault(const String &name, const Variant &defaultValue)
	{

	}

	Variant Serializable::GetInstanceDefault(const String &name) const
	{
		if(instanceDefaultValues_)
		{

		}
	}
}