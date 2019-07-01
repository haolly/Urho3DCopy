//
// Created by liuhao1 on 2017/12/19.
//

#include "XMLFile.h"
#include "PugiXml/src/pugixml.hpp"
#include "../Core/Context.h"
#include "../IO/Deserializer.h"
#include "../IO/Log.h"
#include "../Container/ArrayPtr.h"
#include "ResourceCache.h"
#include "../IO/MemoryBuffer.h"

namespace Urho3D
{

	class XMLWriter : public pugi::xml_writer
	{
	public:
		XMLWriter(Serializer& dest) :
				dest_(dest),
				success_(true)
		{
		}

		virtual void write(const void* data, size_t size) override
		{
			if(dest_.Write(data, (unsigned)size) != size)
				success_ = false;
		}

		Serializer& dest_;
		bool success_;

	};

	XMLFile::XMLFile(Context *context) :
			Resource(context),
			document_(new pugi::xml_document())
	{
	}

	XMLFile::~XMLFile()
	{

	}

	void XMLFile::RegisterObject(Context *context)
	{
		context->RegisterFactory<XMLFile>();
	}

	bool XMLFile::BeginLoad(Deserializer &source)
	{
		unsigned dataSize = source.GetSize();
		if(!dataSize || !source.GetName().Empty())
		{
			URHO3D_LOGERROR("Zero sized XML data in " + source.GetName());
			return false;
		}

		SharedArrayPtr<char> buffer(new char[dataSize]);
		if(source.Read(buffer.Get(), dataSize) != dataSize)
			return false;

		if(!document_->load_buffer(buffer.Get(), dataSize))
		{
			URHO3D_LOGERROR("Could not parse XML data from " + source.GetName());
			document_->reset();
			return false;
		}

		XMLElement rootElem = GetRoot();
		String inherit = rootElem.GetAttribute("inherit");
		if(!inherit.Empty())
		{
			// The existing of this attribute indicates this is an RFC 5261 patch file
			ResourceCache* cache = GetSubsystem<ResourceCache>();
			XMLFile* inheritedXMLFile = static_cast<XMLFile *>(GetAsyncLoadState() == ASYNC_DONE ? cache->GetResource<XMLFile>(inherit) :
			                                                   cache->GetTempResource<XMLFile>(inherit));
			if(!inheritedXMLFile)
			{
				URHO3D_LOGERROR("Could not find inherited XML file: " + inherit);
				return false;
			}

			//UniquePtr<pugi::xml_document> patchDocument(document_.Detach());
			document_ = new pugi::xml_document();
			document_->reset(*inheritedXMLFile->document_);
			Patch(rootElem);

			// Store resource dependencies so we know when to reload / repatch when the inherited resource changes
			cache->StoreResourceDependency(this, inherit);

			dataSize += inheritedXMLFile->GetMemoryUse();
		}

		SetMemoryUse(dataSize);
		return true;
	}

	bool XMLFile::Save(Serializer &dest) const
	{
		return Save(dest, "\t");
	}

	bool XMLFile::Save(Serializer &dest, const String &indentation) const
	{
		XMLWriter writer(dest);
		document_->save(writer, indentation.CString());
		return writer.success_;
	}

	bool XMLFile::FromString(const String &source)
	{
		if(source.Empty())
			return false;
		MemoryBuffer buffer(source.CString(), source.Length());
		return Load(buffer);
	}

	XMLElement XMLFile::CreateRoot(const String &name)
	{
		document_->reset();
		pugi::xml_node root = document_->append_child(name.CString());
		return XMLElement(this, root.internal_object());
	}

	XMLElement XMLFile::GetOrCreateRoot(const String &name)
	{
		XMLElement root = GetRoot(name);
		if(root.NotNull())
			return root;
		root = GetRoot();
		if(root.NotNull())
			URHO3D_LOGWARNING("XMLFile already has root " + root.GetName() + ", deleting it and creating root " + name);
		return CreateRoot(name);
	}

	XMLElement XMLFile::GetRoot(const String &name)
	{
		pugi::xml_node root = document_->first_child();
		if(root.empty())
			return XMLElement();
		if(!name.Empty() && name != root.name())
			return XMLElement();
		else
			return XMLElement(this, root.internal_object());
	}

	String XMLFile::ToString(const String &indentation) const
	{
		//todo
	}

	void XMLFile::Patch(XMLFile *patchFile)
	{
		Patch(patchFile->GetRoot());
	}

	void XMLFile::Patch(XMLElement patchElement)
	{
		pugi::xml_node root = pugi::xml_node(patchElement.GetNode());

		for(auto iter = root.begin(); iter != root.end(); ++iter)
		{
			pugi::xml_attribute sel = iter->attribute("sel");
			if(sel.empty())
			{
				URHO3D_LOGERROR("XML Patch failed due to node not having a sel attribute.");
				continue;
			}

			auto originalNode = document_->select_single_node(sel.value());
			if(!originalNode)
			{
				URHO3D_LOGERRORF("XML Patch failed with bad select : %s ", sel.value());
				continue;
			}

			if(strcmp(iter->name(), "add") == 0)
				PatchAdd(*iter, originalNode);
			else if(strcmp(iter->name(), "replace") == 0)
				PatchReplace(*iter, originalNode);
			else if(strcmp(iter->name(), "remove") == 0)
				PatchRemove(*iter);
			else
				URHO3D_LOGERROR("XMLFiles used for patching should only use 'add', 'replace' or 'remove' elements");
		}
	}

	void XMLFile::PatchAdd(const pugi::xml_node &patch, pugi::xpath_node &original) const
	{
		if(original.attribute())
		{
			URHO3D_LOGERRORF("XML Patch failed calling Add due to not selecting a node, %s attribute was selected",
							original.attribute().name());
			return;
		}

		pugi::xml_attribute type = patch.attribute("type");
		if(!type || strlen(type.value()) <= 0)
			AddNode(patch, original);
		else if(type.value()[0] == '@')
			AddAttribute(patch, original);
	}

	void XMLFile::PatchReplace(const pugi::xml_node &patch, pugi::xpath_node &original) const
	{
		if(!original.attribute() && original.node())
		{
			pugi::xml_node parent = original.node().parent();
			parent.insert_copy_before(patch.first_child(), original.node());
			parent.remove_child(original.node());
		}
		else if(original.attribute())
		{
			original.attribute().set_value(patch.child_value());
		}
	}

	void XMLFile::PatchRemove(const pugi::xpath_node &original) const
	{
		if(!original.attribute() && original.node())
		{
			pugi::xml_node parent = original.parent();
			parent.remove_child(original.node());
		}
		else if(original.attribute())
		{
			pugi::xml_node parent = original.parent();
			parent.remove_attribute(original.attribute());
		}
	}

	void XMLFile::AddNode(const pugi::xml_node &patch, const pugi::xpath_node &original) const
	{
		pugi::xml_attribute pos = patch.attribute("pos");
		if(!pos || strlen(pos.value()) <= 0 || strcmp(pos.value(), "append") == 0)
		{
			auto iter_start = patch.begin();
			auto iter_end = patch.end();

			//There can not be two consecutive text nodes, so check to see if they need to be combined,
			// If they have been we can skip the first node of the nodes to add
			if(CombineText(patch.first_child(), original.node().last_child(), false))
				iter_start++;

			for(; iter_start != iter_end; ++iter_start)
				original.node().append_copy(*iter_start);
		}
		else if(strcmp(pos.value(), "prepend") == 0)
		{
			auto iter_begin = patch.begin();
			auto iter_end = patch.end();

			//There can not be two consecutive text nodes, so check to see if they need to be combined,
			// If they have been we can skip the first node of the nodes to add
			if(CombineText(patch.last_child(), original.node().first_child(), true))
				iter_end--;

			auto pos = original.node().first_child();
			for(; iter_begin != iter_end; ++iter_begin)
				original.node().insert_copy_before(*iter_begin, pos);
		}
		else if(strcmp(pos.value(), "before") == 0)
		{
			auto iter_begin = patch.begin();
			auto iter_end = patch.end();

			if(CombineText(patch.first_child(), original.node().previous_sibling(), false))
				++iter_begin;

			if(CombineText(patch.last_child(), original.node(), true))
				--iter_end;

			for(; iter_begin != iter_end; ++iter_begin)
				original.parent().insert_copy_before(*iter_begin, original.node());
		}
		else if(strcmp(pos.value(), "after") == 0)
		{
			auto iter_begin = patch.begin();
			auto iter_end = patch.end();

			if(CombineText(patch.first_child(), original.node(), false))
				++iter_begin;

			if(CombineText(patch.last_child(), original.node().next_sibling(), true))
				--iter_end;

			auto pos = original.node();
			for(; iter_begin != iter_end; ++iter_begin)
				pos = original.parent().insert_copy_after(*iter_begin, pos);
		}
	}

	void XMLFile::AddAttribute(const pugi::xml_node &patch, const pugi::xpath_node &original) const
	{
		pugi::xml_attribute attribute = patch.attribute("type");
		if(!patch.first_child() && patch.first_child().type() != pugi::node_pcdata)
		{
			URHO3D_LOGERRORF("XML Patch failed calling Add due to attempting to add non text to an attribute for %s", attribute.value());
			return;
		}
		String name(attribute.value());
		//Remove '@'
		name = name.SubString(1);

		auto newAttribute = original.node().append_attribute(name.CString());
		newAttribute.set_value(patch.child_value());
	}

	bool XMLFile::CombineText(const pugi::xml_node &patch, const pugi::xml_node &original, bool prepend) const
	{
		if(!patch || !original)
			return false;

		if((patch.type() == pugi::node_pcdata && original.type() == pugi::node_pcdata) ||
				(patch.type() == pugi::node_cdata && original.type() == pugi::node_cdata))
		{
			if(prepend)
				const_cast<pugi::xml_node&>(original).set_value(Urho3D::ToString("%s%s", patch.value(), original.value()).CString());
			else
				const_cast<pugi::xml_node&>(original).set_value(Urho3D::ToString("%s%s", original.value(), patch.value()).CString());
			return true;
		}
		return false;
	}
}