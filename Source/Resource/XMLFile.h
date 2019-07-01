//
// Created by liuhao1 on 2017/12/18.
//

#ifndef URHO3DCOPY_XMLFILE_H
#define URHO3DCOPY_XMLFILE_H

#include "Resource.h"
#include "XMLElement.h"

namespace pugi
{
	class xml_document;
	class xml_node;
	class xpath_node;
}

namespace Urho3D
{
	class XMLFile : public Resource
	{
		URHO3D_OBJECT(XMLFile, Resource);
	public:
		XMLFile(Context* context);
		virtual ~XMLFile() override ;

		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;
		virtual bool Save(Serializer& dest) const override ;
		bool Save(Serializer& dest, const String& indentation) const;

		bool FromString(const String& source);
		XMLElement CreateRoot(const String& name);
		XMLElement GetOrCreateRoot(const String& name);
		XMLElement GetRoot(const String& name = String::EMPTY);

		pugi::xml_document* GetDocument() const { return document_.Get(); }

		String ToString(const String& indentation = "\t") const;

		void Patch(XMLFile* patchFile);
		void Patch(XMLElement patchElement);

	private:
		void PatchAdd(const pugi::xml_node& patch, pugi::xpath_node& original) const;
		void PatchReplace(const pugi::xml_node& patch, pugi::xpath_node& original) const;
		void PatchRemove(const pugi::xpath_node& original) const;

		void AddNode(const pugi::xml_node& patch, const pugi::xpath_node& original) const;
		void AddAttribute(const pugi::xml_node& patch, const pugi::xpath_node& original) const;
		bool CombineText(const pugi::xml_node& patch, const pugi::xml_node& original, bool prepend) const;

		UniquePtr<pugi::xml_document> document_;

	};
}

#endif //URHO3DCOPY_XMLFILE_H
