//
// Created by liuhao on 2017/12/20.
//

#include "Serializable.h"
#include "../IO/Log.h"
#include "../IO/Deserializer.h"
#include "../Core/Context.h"
#include "../IO/Serializer.h"
#include "../Resource/XMLElement.h"
#include "SceneEvents.h"

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
				dest = *(reinterpret_cast<const long long*>(src));
				break;
			case VAR_BOOL:
				dest = *(reinterpret_cast<const bool*>(src));
				break;
			case VAR_FLOAT:
				dest = *(reinterpret_cast<const float*>(src));
				break;
			case VAR_VECTOR2:
				dest = *(reinterpret_cast<const Vector2*>(src));
				break;
			case VAR_VECTOR3:
				dest = *(reinterpret_cast<const Vector3*>(src));
				break;
			case VAR_VECTOR4:
				dest = *(reinterpret_cast<const Vector4*>(src));
				break;
				//todo
		}
	}

	const Vector<AttributeInfo> *Serializable::GetAttributes() const
	{
		return context_->GetAttributes(GetType());
	}

	const Vector<AttributeInfo> *Serializable::GetNetworkAttributes() const
	{
		return networkState_ ? networkState_->attributes_ : context_->GetNetworkAttributes(GetType());
	}

	bool Serializable::Load(Deserializer &source)
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
		}
		return true;
	}

	bool Serializable::Save(Serializer &dest) const
	{
		const Vector<AttributeInfo>* attributes = GetAttributes();
		if(!attributes)
			return true;

		Variant value;
		for(unsigned i=0; i< attributes->Size(); ++i)
		{
			const AttributeInfo& attr = attributes->At(i);
			if(!(attr.mode_ & AM_FILE) || (attr.mode_ & AM_FILEREADONLY) == AM_FILEREADONLY)
				continue;
			OnGetAttribute(attr, value);

			if(!dest.WriteVariant(value))
			{
				URHO3D_LOGERROR("Could not save " + GetTypeName() + ", writing to stream failed");
				return false;
			}
		}
		return true;
	}

	bool Serializable::LoadXML(const XMLElement &source)
	{
		if(source.IsNull())
		{
			URHO3D_LOGERROR("Could not load " + GetTypeName() + ", null source element");
			return false;
		}
		const Vector<AttributeInfo>* attributes = GetAttributes();
		if(!attributes)
			return true;
		XMLElement attrElem = source.GetChild("attribute");
		unsigned startIndex = 0;
		while(attrElem)
		{
			String name = attrElem.GetAttribute("name");
			unsigned i = startIndex;
			unsigned attempts = attributes->Size();
			while (attempts)
			{
				const AttributeInfo& attr = attributes->At(i);
				if((attr.mode_ & AM_FILE) && !attr.name_.Compare(name, true))
				{
					Variant varValue;

					// If enums specified, do enum lookup and int assignment. Otherwise assign the variant
					if(attr.enumNames_)
					{
						String value = attrElem.GetAttribute("value");
						bool enumFound = false;
						int enumValue = 0;
						const char** enumPtr = attr.enumNames_;
						while (*enumPtr)
						{
							if(!value.Compare(*enumPtr, false))
							{
								enumFound = true;
								break;
							}
							++enumPtr;
							++enumValue;
						}
						if(enumFound)
							varValue = enumValue;
						else
							URHO3D_LOGWARNING("Unknow enum value " + value + " in attribute " + attr.name_);

					}
					else
					{
						varValue = attrElem.GetVariantValue(attr.type_);
					}

					if(!varValue.IsEmpty())
					{
						OnSetAttribute(attr, varValue);
					}

					startIndex = (i + 1) % attributes->Size();
					// load next attribute name
					break;
				}
				else
				{
					//Search next attribute name
					i = (i+1) % attributes->Size();
					--attempts;
				}
			}

			// Do not found the corresponding attribute
			if(!attempts)
				URHO3D_LOGWARNING("Unknown attribute " + name + " in XML data");

			attrElem = attrElem.GetNext("attribute");
		}
		return true;
	}

	// Save attributes in here
	bool Serializable::SaveXML(XMLElement &dest) const
	{
		if(dest.IsNull())
		{
			URHO3D_LOGERROR("Could not save " + GetTypeName() + ", null destination element");
			return false;
		}

		const Vector<AttributeInfo>* attributes = GetAttributes();
		if(!attributes)
			return true;

		Variant value;
		for(unsigned i=0; i< attributes->Size(); ++i)
		{
			const AttributeInfo& attr = attributes->At(i);
			if(!(attr.mode_ & AM_FILE) || (attr.mode_ & AM_FILEREADONLY) == AM_FILEREADONLY)
				continue;

			OnGetAttribute(attr, value);
			Variant defaultValue(GetAttributeDefault(i));

			if((value == defaultValue) && !SaveDefaultAttributes())
				continue;

			XMLElement attrElem = dest.CreateChild("attribute");
			attrElem.SetAttribute("name", attr.name_);

			// If enums specified, set as an enum string, Otherwise set directly as a Variant
			if(attr.enumNames_)
			{
				int enumValues = value.GetInt();
				attrElem.SetAttribute("value", attr.enumNames_[enumValues]);
			}
			else
				attrElem.SetVariantValue(value);
		}
		return true;
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
			if(attr.mode_ & (AM_NOEDIT | AM_NODEID | AM_COMPONENTID | AM_NODEIDVECTOR))
				continue;

			Variant defaultValue = GetInstanceDefault(attr.name_);
			if(defaultValue.IsEmpty())
				defaultValue = attr.defaultValue_;
			OnSetAttribute(attr, defaultValue);
		}
	}

	void Serializable::RemoveInstanceDefault()
	{
		instanceDefaultValues_.Reset();
	}

	void Serializable::SetTemporary(bool enable)
	{
		if(enable != temporary_)
		{
			temporary_ = enable;

			using namespace TemporaryChanged;
			VariantMap& eventData = GetEventDataMap();
			eventData[P_SERIALIZABLE] = this;
			SendEvent(E_TEMPORARYCHANGED, eventData);
		}
	}

	void Serializable::SetInterceptNewwrokUpdate(const String &attributeName, bool enable)
	{
		AllocateNetworkState();
		//todo
	}

	void Serializable::AllocateNetworkState()
	{
		if(networkState_)
			return;

		const Vector<AttributeInfo>* networkStates = GetNetworkAttributes();
		networkState_ = new NetworkState();
		networkState_->attributes_ = networkStates;

		if(!networkStates)
			return;
		unsigned numAttributes = networkStates->Size();
		if(networkState_->currentValues_.Size() != numAttributes)
		{
			networkState_->currentValues_.Resize(numAttributes);
			networkState_->previousValues_.Resize(numAttributes);

			// Copy the default attribute values to the previous state as a starting point
			for(unsigned i=0; i< numAttributes; ++i)
			{
				//todo
			}
		}
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
		if(!instanceDefaultValues_)
			instanceDefaultValues_ = new VariantMap();
		(*instanceDefaultValues_)[name] = defaultValue;
	}

	Variant Serializable::GetInstanceDefault(const String &name) const
	{
		if(instanceDefaultValues_)
		{
			auto it = instanceDefaultValues_->Find(name);
			if(it != instanceDefaultValues_->End())
				return it->second_;
		}
		return Variant::EMPTY;
	}

	bool Serializable::ReadDeltaUpdate(Deserializer &source)
	{
		//todo
	}

	bool Serializable::ReadLatestDataUpdate(Deserializer &source)
	{
		return false;
	}

	Variant Serializable::GetAttribute(unsigned index) const
	{
		Variant ret;
		const Vector<AttributeInfo>* attributes = GetAttributes();
		if(!attributes)
		{
			URHO3D_LOGERROR(GetTypeName() + " has no attributes");
			return ret;
		}
		if(index >= attributes->Size())
		{
			URHO3D_LOGERROR("Attribute index out of bounds");
			return ret;
		}
		OnGetAttribute(attributes->At(index), ret);
		return ret;
	}

	Variant Serializable::GetAttribute(const String &name) const
	{
		return Variant();
	}

	Variant Serializable::GetAttributeDefault(unsigned index) const
	{
		return Variant();
	}

	Variant Serializable::GetAttributeDefault(const String &name) const
	{
		return Variant();
	}

	unsigned Serializable::GetNumAttributes() const
	{
		return 0;
	}

	unsigned Serializable::GetNumNetworkAttributes() const
	{
		return 0;
	}

	bool Serializable::GetInterceptNetworkUpdate(const String &attributeName) const
	{
		return false;
	}
}