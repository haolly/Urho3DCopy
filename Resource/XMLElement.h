//
// Created by liuhao1 on 2017/12/19.
//

#ifndef URHO3DCOPY_XMLELEMENT_H
#define URHO3DCOPY_XMLELEMENT_H

#include "../Container/Str.h"
#include "../Container/Ptr.h"
#include "../Math/Color.h"

namespace pugi
{
	struct xml_node_struct;
	class xpath_node;
	class xpath_node_set;
	class xpath_query;
	class xpath_variable_set;
}

namespace Urho3D
{
	class XMLFile;
	class XPathQuery;
	class XPathResultSet;

	//Note, 需要先看一遍文档，不然不知道相关的类是干啥用的
	//ref https://pugixml.org/docs/quickstart.html
	class XMLElement
	{
	public:
		XMLElement();
		XMLElement(XMLFile* file, pugi::xml_node_struct* node);
		XMLElement(XMLFile* file, const XPathResultSet* resultSet, const pugi::xpath_node* xpathNode, unsigned xpathResultIndex);
		XMLElement(const XMLElement& rhs);

		~XMLElement();

		XMLElement&operator =(const XMLElement& rhs);

		XMLElement CreateChild(const String& name);
		XMLElement CreateChild(const char* name);
		XMLElement GetOrCreateChild(const String& name);
		XMLElement GetOrCreateChild(const char* name);

		bool AppendChild(XMLElement element, bool asCopy = false);
		bool Remove();
		bool RemoveChild(XMLElement& element);
		bool RemoveChild(const String& name);
		bool RemoveChild(const char* name);

		bool RemoveChildren(const String& name = String::EMPTY);
		bool RemoveChildren(const char* name);

		bool RemoveAttribute(const String& name = String::EMPTY);
		bool RemoveAttribute(const char* name);

		XMLElement SelectSingle(const String& query, pugi::xpath_variable_set* variable = nullptr) const;
		XMLElement SelectSinglePrepared(const XPathQuery& query) const;
		XPathResultSet Select(const String& query, pugi::xpath_variable_set* variables = nullptr) const;
		XPathResultSet SelectPrepared(const XPathQuery& query) const;

		bool SetValue(const String& value);
		bool SetValue(const char* value);
		bool SetAttribute(const String& name, const String& value);
		bool SetAttribute(const char* name, const char* value);
		bool SetAttribute(const String& value);
		bool SetAttribute(const char* value);

		bool SetBool(const String& name, bool value);
		bool SetBuffer(const String& name, const void* data, unsigned size);
		bool SetBuffer(const String& name, const PODVector<unsigned char>& value);

		bool SetFloat(const String& name, float value);
		bool SetDouble(const String& name, double value);
		bool SetUInt(const String& name, unsigned value);
		bool SetInt(const String& name, int value);
		bool SetUInt64(const String& name, unsigned long long value);
		//todo

		bool IsNull() const;
		bool NotNull() const;

		operator bool() const;

		String GetName() const;
		bool HasChild(const String& name) const;
		bool HasChild(const char* name) const;

		XMLElement GetChild(const String& name = String::EMPTY) const;
		XMLElement GetChild(const char* name) const;
		XMLElement GetNext(const String& name = String::EMPTY) const;
		XMLElement GetNext(const char* name) const;
		XMLElement GetParent() const;

		unsigned GetNumAttributes() const;
		bool HasAttributes(const String& name) const;
		bool HasAttributes(const char* name) const;


		String GetValue() const;
		String GetAttribute(const String& name = String::EMPTY) const;
		String GetAttribute(const char* name) const;
		const char* GetAttributeCString(const char* name) const;
		String GetAttributeLower(const String& name) const;
		String GetAttributeLower(const char* name) const;
		String GetAttributeUpper(const String& name) const;
		String GetAttributeUpper(const char* name) const;
		Vector<String> GetAttributeNames() const;
		bool GetBool(const String& name) const;
		PODVector<unsigned char> GetBuffer(const String& name) const;
		bool GetBuffer(const String& name, void* dest, unsigned size) const;
		BoundingBox GetBoundingBox() const;
		Color GetColor(const String& name) const;
		float GetFloat(const String& name) const;
		//todo



		XMLFile* GetFile() const;

		pugi::xml_node_struct* GetNode() const
		{
			return node_;
		}

		const XPathResultSet* GetXPathResultSet() const
		{
			return xpathResultSet_;
		}

		const pugi::xpath_node* GetXPathNode() const
		{
			return xpathNode_;
		}

		unsigned GetXPathResultIndex() const
		{
			return xpathResultIndex_;
		}

		XMLElement NextResult() const;

		static const XMLElement EMPTY;


	private:
		WeakPtr<XMLFile> file_;
		pugi::xml_node_struct* node_;
		const XPathResultSet* xpathResultSet_;
		const pugi::xpath_node* xpathNode_;
		mutable unsigned xpathResultIndex_;
	};

	class XPathResultSet
	{
		XPathResultSet();
		XPathResultSet(XMLFile* file, pugi::xpath_node_set* resultSet);
		XPathResultSet(const XPathResultSet& rhs);
		~XPathResultSet();
		XPathResultSet&operator =(const XPathResultSet& rhs);
		XMLElement operator[](unsigned index) const;
		XMLElement FirstResult();
		unsigned Size() const;
		bool Empty() const;
		pugi::xpath_node_set* GetXPathNodeSet() const
		{
			return resultSet_;
		}
	private:
		WeakPtr<XMLFile> file_;
		pugi::xpath_node_set* resultSet_;
	};

	class XPathQuery
	{
	public:
		XPathQuery();
		XPathQuery(const String& queryString, const String& variableString = String::EMPTY);
		~XPathQuery();

		void Bind();
		void SetVariable(const String& name, bool value);
		void SetVariable(const String& name, float value);
		void SetVariable(const String& name, const String& value);
		void SetVariable(const char* name, const XPathResultSet& value);
		bool SetQuery(const String& queryString, const String& variableString = String::EMPTY, bool bind = true);
		void Clear();
		//todo


	private:
		String queryString_;
		UniquePtr<pugi::xpath_query> query_;
		UniquePtr<pugi::xpath_variable_set> variables_;
	};
}

#endif //URHO3DCOPY_XMLELEMENT_H
