//
// Created by liuhao on 2017/12/20.
//

#ifndef URHO3DCOPY_SERIALIZABLE_H
#define URHO3DCOPY_SERIALIZABLE_H

#include "../Core/Object.h"
#include "../Core/Attribute.h"
#include "ReplicationState.h"

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
		bool ReadDeltaUpdate(Deserializer& source);
		bool ReadLatestDataUpdate(Deserializer& source);

		Variant GetAttribute(unsigned index) const;
		Variant GetAttribute(const String& name) const;
		Variant GetAttributeDefault(unsigned index) const;
		Variant GetAttributeDefault(const String& name) const;
		unsigned GetNumAttributes() const;
		unsigned GetNumNetworkAttributes() const;
		bool IsTemporary() const { return temporary_; }
		bool GetInterceptNetworkUpdate(const String& attributeName) const;
		NetworkState* GetNetworkState() const { return networkState_.Get(); }
	protected:
		UniquePtr<NetworkState> networkState_;
	private:
		void SetInstanceDefault(const String& name, const Variant& defaultValue);
		Variant GetInstanceDefault(const String& name) const;

		UniquePtr<VariantMap> instanceDefaultValues_;
		// todo, what is the point?
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

	template <class TClassType, class TGetFunction, class TSetFunction>
	SharedPtr<AttributeAccessor> MakeVariantAttributeAccessor(TGetFunction getFunction, TSetFunction setFunction)
	{
		return SharedPtr<AttributeAccessor>(new VariantAttributeAccessorImpl<TClassType, TGetFunction, TSetFunction>(getFunction, setFunction));
	};

#define URHO3D_MAKE_MEMBER_ATTRIBUTE_ACCESSOR(typeName, variable) Urho3D::MakeVariantAttributeAccessor<ClassName>( \
	[](const ClassName& self, Urho3D::Variant& value) {value = self.variable;}, \
	[](ClassName& self, const Urho3D::Variant& value) {self.variable = value.Get<typeName>();})

#define URHO3D_MAKE_MEMBER_ATTRIBUTE_ACCESSOR_EX(typeName, variable, postSetCallback) Urho3D::MakeVariantAttributeAccessor<ClassName>( \
	[](const ClassName& self, Urho3D::Variant& value) { value = self.variable;}, \
	[](ClassName& self, const Urho3D::Variant& value) { self.variable = value.Get<typeName>(); self.postSetCallback(); })

	//todo
#define URHO3D_ATTRIBUTE(name, typeName, variable, defaultValue, mode) context->RegisterAttribute<ClassName>(Urho3D::AttributeInfo(\
	Urho3D::GetVariantType<typeName>(), name, URHO3D_MAKE_MEMBER_ATTRIBUTE_ACCESSOR(typeName, variable), nullptr, defaultValue, mode))

#define URHO3D_ATTRIBUTE_EX(name, typeName, variable, postSetCallback, defaultValue, mode) context->RegisterAttribute<ClassName>(Urho3D::AttributeInfo( \
	Urho3D::GetVariantType<typeName>(), name, URHO3D_MAKE_MEMBER_ATTRIBUTE_ACCESSOR_EX(typeName, variable, postSetCallback), nullptr, defaultValue, mode))

//todo
#define URHO3D_MAKE_GET_SET_ATTRIBUTE_ACCESSOR(getFunction, setFunction, typeName) Urho3D::MakeVariantAttributeAccessor<ClassName>( \
	[](const ClassName& self, Urho3D::Variant& value) { value = self.getFunction(); }, \
	[](ClassName& self, const Urho3D::Variant& value) { self.setFunction(value.Get<typeName>()); })

	//Note, 这里的 ClassName 从哪里传来的？？
	//从 URHO3D_OBJECT 中来
#define URHO3D_ACCESSOR_ATTRIBUTE(name, getFunction, setFunction, typeName, defaultValue, mode) context->RegisterAttribute<ClassName>(Urho3D::AttributeInfo( \
	Urho3D::GetVariantType<typeName>(), name, URHO3D_MAKE_GET_SET_ATTRIBUTE_ACCESSOR(getFunction, setFunction, typeName), nullptr, defaultValue, mode))

// Deprecated, Use URHO3D_ACCESSOR_ATTRIBUTE instead
#define URHO3D_MIXED_ACCESSOR_ATTRIBUTE(name, getFunction, setFunction, typeName, defaultValue, mode) URHO3D_ACCESSOR_ATTRIBUTE( \
		name, getFunction, setFunction, typeName, defaultValue, mode)

#endif //URHO3DCOPY_SERIALIZABLE_H
}


