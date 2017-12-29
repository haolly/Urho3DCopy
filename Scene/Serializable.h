//
// Created by liuhao on 2017/12/20.
//

#ifndef URHO3DCOPY_SERIALIZABLE_H
#define URHO3DCOPY_SERIALIZABLE_H

#include "../Core/Object.h"
#include "../Core/Attribute.h"

namespace Urho3D
{
	class Deserializer;
	class Serializer;
	class XMLElement;

	class Serializable : public Object
	{
		URHO3D_OBJECT(Serializable, Object);
	public:
		Serializable(Context* context);

		virtual ~Serializable() override;

		virtual void OnSetAttribute(const AttributeInfo& attr, const Variant& src);
		virtual void OnGetAttribute(const AttributeInfo& attr, Variant& dest) const;

		virtual const Vector<AttributeInfo>* GetAttributes() const;
		virtual const Vector<AttributeInfo>* GetNetworkAttributes() const;

		virtual bool Load(Deserializer& source, bool setInstanceDefault = false);
		virtual bool Save(Serializer& dest) const;

		virtual bool LoadXML(const XMLElement& source, bool setInstanceDefault = false);
		virtual bool SaveXML(XMLElement& dest) const;

		//todo

		virtual void ApplyAttributes() {}

		virtual bool SaveDefaultAttributes() const { return false; }

		virtual void MarkNetworkUpdate() {}

		bool SetAttribute(unsigned index, const Variant& value);
		bool SetAttribute(const String& name, const Variant& value);
		void ResetToDefault();
		void RemoveInstanceDefault();
		void SetTemporary(bool enable);
		void SetInterceptNewwrokUpdate(const String& attributeName, bool enable);
		void AllocateNetworkState();
		void WriteInitialDeltaUpdate(Serializer& dest, const DirtyBits& attributeBits, unsigned char timeStemp);
		void WriteLatestDataUpdate(Serializer& dest, unsigned char timeStamp);
		//todo
	protected:
		UniquePtr<NetworkState> networkState_;
	private:
		void SetInstanceDefault(const String& name, const Variant& defaultValue);
		Variant GetInstanceDefault(const String& name) const;

		UniquePtr<VariantMap> instanceDefaultValues_;
		bool temporary_;
	};

	template <class TClassType, class TGetFunction, class TSetFunction>
	class VariantAttributeAccessorImpl : public AttributeAccessor
	{
	public:
		VariantAttributeAccessorImpl(TGetFunction getFunction, TSetFunction setFunction) :
				getFunction_(getFunction),
				setFunction_(setFunction)
		{
		}

		void Get(const Serializable *ptr, Variant &dest) const override
		{
			assert(ptr);
			const auto classPtr = static_cast<const TClassType*>(ptr);
			getFunction_(*classPtr, dest);
		}

		void Set(Serializable *ptr, const Variant &src) override
		{
			assert(ptr);
			auto classPtr = static_cast<TClassType*>(ptr);
			setFunction_(*classPtr, src);
		}


	private:
		TGetFunction getFunction_;
		TSetFunction setFunction_;
	};

}



#endif //URHO3DCOPY_SERIALIZABLE_H
