//
// Created by liuhao1 on 2017/12/24.
//

#include "VertexBuffer.h"

namespace Urho3D
{

	VertexBuffer::VertexBuffer(Context *context, bool forceHandless) :
		Object(context),
		GPUObject(forceHandless ? nullptr : GetSubsystem<Graphics>()),
		vertexCount_(0),
		elementMask_(0),
		lockState_(LOCK_NONE),
		lockStart_(0),
		lockCount_(0),
		lockScratchData_(nullptr),
		shadowed_(false),
		dynamic_(false),
		discardLock_(false)
	{
		UpdateOffset();
		if(!graphics_)
			shadowed_ = true;
	}

	VertexBuffer::~VertexBuffer()
	{
		Release();
	}


	void VertexBuffer::SetShadowed(bool enable)
	{
		if(!graphics_)
			enable = true;
		if(enable != shadowed_)
		{
			if(enable && vertexSize_ && vertexCount_)
				shadowData_ = new unsigned char[vertexCount_ * vertexSize_];
			else
				shadowData_.Reset();

			shadowed_ = enable;
		}
	}

	bool VertexBuffer::SetSize(unsigned vertexCount, const PODVector<VertexElement> &element, bool dynamic)
	{
		Unlock();

		vertexCount_ = vertexCount;
		elements_ = element;
		dynamic_ = dynamic;

		UpdateOffset();

		if(shadowed_ && vertexCount_ && vertexSize_)
			shadowData_ = new unsigned char[vertexCount_ * vertexSize_];
		else
			shadowData_.Reset();

		return Create();
	}

	bool VertexBuffer::SetSize(unsigned vertexCount, unsigned elementMask, bool dynamic)
	{
		return SetSize(vertexCount, GetElements(elementMask), dynamic);
	}

	bool VertexBuffer::SetData(const void *data)
	{
		return false;
	}

	bool VertexBuffer::SetDataRange(const void *data, unsigned start, unsigned count, bool discard)
	{
		return false;
	}



	const VertexElement *VertexBuffer::GetElement(VertexElementSemantic semantic, unsigned char index) const
	{
		for(auto it = elements_.Begin(); it != elements_.End(); ++it)
		{
			if(it->semantic_ == semantic && it->index_ == index)
				return &(*it);
		}
		return nullptr;
	}

	const VertexElement *
	VertexBuffer::GetElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index) const
	{
		for(auto it = elements_.Begin(); it != elements_.End(); ++it)
		{
			if(it->type_ == type && it->semantic_ == semantic && it->index_ == index)
				return &(*it);
		}
		return nullptr;
	}

	const VertexElement *VertexBuffer::GetElement(const PODVector<VertexElement> &elements, VertexElementType type,
	                                              VertexElementSemantic semantic, unsigned char index)
	{
		for(auto it = elements.Begin(); it != elements.End(); ++it)
		{
			if(it->type_ == type && it->semantic_ == semantic && it->index_ == index)
				return &(*it);
		}
		return nullptr;
	}

	bool VertexBuffer::HasElement(const PODVector<VertexElement> &elements, VertexElementType type,
	                              VertexElementSemantic semantic, unsigned char index)
	{
		return GetElement(elements, type, semantic, index) != nullptr;
	}

	//Todo, each element has the property offset_, this can be used to get uvOffset, Geometry::GetHitDistance
	unsigned VertexBuffer::GetElementOffset(const PODVector<VertexElement> &elements, VertexElementType type,
	                                        VertexElementSemantic semantic, unsigned char index)
	{
		const VertexElement* element = GetElement(elements, type, semantic, index);
		return element ? element->offset_ : M_MAX_UNSIGNED;
	}

	PODVector<VertexElement> VertexBuffer::GetElements(unsigned elementMask)
	{
		PODVector<VertexElement> ret;
		for(unsigned i=0; i< MAX_LEGACY_VERTEX_ELEMENTS; ++i)
		{
			if(elementMask & (i << i))
				ret.Push(LEGACY_VERTEXELEMENTS[i]);
		}
	}

	unsigned VertexBuffer::GetVertexSize(const PODVector<VertexElement> &elements)
	{
		unsigned size = 0;
		for(unsigned i= 0; i< elements.Size(); ++i)
		{
			size += ELEMENT_TYPESIZES[elements[i].type_];
		}
		return size;
	}

	unsigned VertexBuffer::GetVertexSize(unsigned elementMask)
	{
		unsigned size = 0;
		for(unsigned i=0; i< MAX_LEGACY_VERTEX_ELEMENTS; ++i)
		{
			if(elementMask & (1 << i))
				size += ELEMENT_TYPESIZES[LEGACY_VERTEXELEMENTS[i].type_];
		}
		return size;
	}

	void VertexBuffer::UpdateOffsets(PODVector<VertexElement> &elements)
	{
		unsigned elementOffset = 0;
		for(auto it = elements.Begin(); it != elements.End(); ++it)
		{
			it->offset_ = elementOffset;
			elementOffset += ELEMENT_TYPESIZES[it->type_];
		}
	}

	void VertexBuffer::UpdateOffset()
	{
		unsigned elementOffset = 0;
		elementHash_ = 0;
		elementMask_ = 0;

		for(auto it = elements_.Begin(); it != elements_.End(); ++it)
		{
			it->offset_ = elementOffset;
			elementOffset += ELEMENT_TYPESIZES[it->type_];
			elementHash_ <<= 6;
			elementHash_ += (((int)it->type_ + 1) * ((int)it->semantic_ + 1) + it->index_);

			for(unsigned i = 0; i< MAX_LEGACY_VERTEX_ELEMENTS; ++i)
			{
				const VertexElement& legacy = LEGACY_VERTEXELEMENTS[i];
				if(it->type_ == legacy.type_ && it->semantic_ == legacy.semantic_ && it->index_ == legacy.index_)
					elementMask_ |= (1 << i);
			}
		}
		//Todo, the total size of all elements ??
		vertexSize_ = elementOffset;
	}



}