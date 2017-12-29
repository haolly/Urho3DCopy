//
// Created by liuhao1 on 2017/12/19.
//
#include "XMLElement.h"
#include "XMLFile.h"
#include "../ThirdParty/PugiXml/src/pugixml.hpp"
#include "../IO/Log.h"

namespace Urho3D
{
	const XMLElement XMLElement::EMPTY;

	XMLElement::XMLElement() :
		node_(nullptr),
		xpathResultSet_(nullptr),
		xpathNode_(nullptr),
		xpathResultIndex_(0)
	{
	}

	XMLElement::XMLElement(XMLFile *file, pugi::xml_node_struct *node) :
		file_(file),
		node_(node),
		xpathResultSet_(nullptr),
		xpathNode_(nullptr),
		xpathResultIndex_(0)
	{
	}

	XMLElement::XMLElement(XMLFile *file, const XPathResultSet *resultSet, const pugi::xpath_node *xpathNode,
	                       unsigned xpathResultIndex) :
		file_(file),
		node_(nullptr),
		xpathResultSet_(resultSet),
		xpathNode_(resultSet ? xpathNode : (xpathNode ? new pugi::xpath_node(*xpathNode) : nullptr)),
		xpathResultIndex_(xpathResultIndex)
	{
	}

	XMLElement::XMLElement(const XMLElement &rhs) :
		file_(rhs.file_),
		node_(rhs.node_),
		xpathResultSet_(rhs.xpathResultSet_),
		xpathNode_(rhs.xpathResultSet_ ? rhs.xpathNode_ : ( rhs.xpathNode_ ? new pugi::xpath_node(*rhs.xpathNode_) : nullptr)),
		xpathResultIndex_(rhs.xpathResultIndex_)
	{
	}

	XMLElement::~XMLElement()
	{
		if(!xpathResultSet_ && xpathNode_)
		{
			delete xpathNode_;
			xpathNode_ = nullptr;
		}
	}

	XMLElement &XMLElement::operator=(const XMLElement &rhs)
	{
		file_ = rhs.file_;
		node_ = rhs.node_;
		xpathResultSet_ = rhs.xpathResultSet_;
		xpathNode_ = rhs.xpathResultSet_ ? rhs.xpathNode_ : (rhs.xpathNode_ ? new pugi::xpath_node(*rhs.xpathNode_) : nullptr);
		xpathResultIndex_ = rhs.xpathResultIndex_;
		return *this;
	}

	XMLElement XMLElement::CreateChild(const String &name)
	{
		return CreateChild(name.CString());
	}

	XMLElement XMLElement::CreateChild(const char *name)
	{
		if(!file_ || (!node_ && !xpathNode_))
			return XMLElement();
		const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() :: pugi::xml_node(node_);
		//todo
	}

	XMLElement XMLElement::GetOrCreateChild(const String &name)
	{
		return XMLElement();
	}

	XMLElement XMLElement::GetOrCreateChild(const char *name)
	{
		return XMLElement();
	}

	bool XMLElement::AppendChild(XMLElement element, bool asCopy)
	{
		return false;
	}

	bool XMLElement::Remove()
	{
		return false;
	}

	bool XMLElement::RemoveChild(XMLElement &element)
	{
		return false;
	}

	bool XMLElement::RemoveChild(const String &name)
	{
		return false;
	}

	bool XMLElement::RemoveChild(const char *name)
	{
		return false;
	}

	bool XMLElement::RemoveChildren(const String &name)
	{
		return false;
	}

	bool XMLElement::RemoveChildren(const char *name)
	{
		return false;
	}

	bool XMLElement::RemoveAttribute(const String &name)
	{
		return false;
	}

	bool XMLElement::RemoveAttribute(const char *name)
	{
		return false;
	}

	XMLElement XMLElement::SelectSingle(const String &query, pugi::xpath_variable_set *variable) const
	{
		return XMLElement();
	}

	XMLElement XMLElement::SelectSinglePrepared(const XPathQuery &query) const
	{
		return XMLElement();
	}

	XPathResultSet XMLElement::Select(const String &query, pugi::xpath_variable_set *variables) const
	{
		return XPathResultSet();
	}

	XPathResultSet XMLElement::SelectPrepared(const XPathQuery &query) const
	{
		return XPathResultSet();
	}

	bool XMLElement::SetValue(const String &value)
	{
		return false;
	}

	bool XMLElement::SetValue(const char *value)
	{
		return false;
	}

	bool XMLElement::SetAttribute(const String &name, const String &value)
	{
		return false;
	}

	bool XMLElement::SetAttribute(const char *name, const char *value)
	{
		return false;
	}

	bool XMLElement::SetAttribute(const String &value)
	{
		return false;
	}

	bool XMLElement::SetAttribute(const char *value)
	{
		return false;
	}

	bool XMLElement::SetBool(const String &name, bool value)
	{
		return false;
	}

	bool XMLElement::SetBuffer(const String &name, const void *data, unsigned size)
	{
		return false;
	}

	bool XMLElement::SetBuffer(const String &name, const PODVector<unsigned char> &value)
	{
		return false;
	}

	bool XMLElement::SetFloat(const String &name, float value)
	{
		return false;
	}

	bool XMLElement::SetDouble(const String &name, double value)
	{
		return false;
	}

	bool XMLElement::SetUInt(const String &name, unsigned value)
	{
		return false;
	}

	bool XMLElement::SetInt(const String &name, int value)
	{
		return false;
	}

	bool XMLElement::SetUInt64(const String &name, unsigned long long value)
	{
		return false;
	}

	bool XMLElement::IsNull() const
	{
		return false;
	}

	bool XMLElement::NotNull() const
	{
		return node_ || (xpathNode_ && !xpathNode_->operator!());
	}

	XMLElement::operator bool() const
	{
		return false;
	}

	String XMLElement::GetName() const
	{
		return String();
	}

	bool XMLElement::HasChild(const String &name) const
	{
		return false;
	}

	bool XMLElement::HasChild(const char *name) const
	{
		return false;
	}

	XMLElement XMLElement::GetChild(const String &name) const
	{
		return XMLElement();
	}

	XMLElement XMLElement::GetChild(const char *name) const
	{
		return XMLElement();
	}

	XMLElement XMLElement::GetNext(const String &name) const
	{
		return XMLElement();
	}

	XMLElement XMLElement::GetNext(const char *name) const
	{
		return XMLElement();
	}

	XMLElement XMLElement::GetParent() const
	{
		return XMLElement();
	}

	unsigned XMLElement::GetNumAttributes() const
	{
		return 0;
	}

	bool XMLElement::HasAttributes(const String &name) const
	{
		return false;
	}

	bool XMLElement::HasAttributes(const char *name) const
	{
		return false;
	}

	String XMLElement::GetValue() const
	{
		return String();
	}

	String XMLElement::GetAttribute(const String &name) const
	{
		return String();
	}

	String XMLElement::GetAttribute(const char *name) const
	{
		return String();
	}

	XMLFile *XMLElement::GetFile() const
	{
		return nullptr;
	}

	XMLElement XMLElement::NextResult() const
	{
		return XMLElement();
	}

	XPathResultSet::XPathResultSet() :
		resultSet_(nullptr)
	{
	}

	XPathResultSet::XPathResultSet(XMLFile *file, pugi::xpath_node_set *resultSet) :
		file_(file),
		resultSet_(resultSet ? new pugi::xpath_node_set(resultSet->begin(), resultSet->end()) : nullptr)
	{
		if(resultSet_)
			resultSet_->sort();
	}

	XPathResultSet::XPathResultSet(const XPathResultSet &rhs) :
		file_(rhs.file_),
		resultSet_(rhs.resultSet_ ? new pugi::xpath_node_set(rhs.resultSet_->begin(), rhs.resultSet_->end()) : nullptr)
	{
	}

	XPathResultSet::~XPathResultSet()
	{
		delete resultSet_;
		resultSet_ = nullptr;
	}

	XPathResultSet &XPathResultSet::operator=(const XPathResultSet &rhs)
	{
		file_ = rhs.file_;
		resultSet_ = rhs.resultSet_ ? new pugi::xpath_node_set(rhs.resultSet_->begin(), rhs.resultSet_->end()) : nullptr;
		return *this;
	}

	XMLElement XPathResultSet::operator[](unsigned index) const
	{
		if(!resultSet_)
			URHO3D_LOGERRORF("Could not return result at index: %u, Most probably this is caused by the XPathResultSet not being"
					                 "stored in lhs variable", index);
		return resultSet_ && index < Size() ? XMLElement(file_, this, &(*resultSet_)[index], index) : XMLElement();
	}

	XMLElement XPathResultSet::FirstResult()
	{
		return operator[](0);
	}

	unsigned XPathResultSet::Size() const
	{
		return resultSet_ ? (unsigned)resultSet_->size() : 0;
	}

	bool XPathResultSet::Empty() const
	{
		return resultSet_ ? resultSet_->empty() : true;
	}

}