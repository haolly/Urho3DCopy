//
// Created by liuhao on 2019/2/23.
//

#include "UIElement.hpp"
#include "../Core/Context.h"
#include "UIEvents.hpp"

namespace Urho3D
{
	const char* horizontalAlignmants[] =  {
			"Left",
			"Center",
			"Right",
			"Custom",
			nullptr
	};

	//todo

	extern const char* UI_CATEGORY;

	UIElement::UIElement(Context *context) :
			Animatable(context),
			pivot_(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())
	{
		SetEnabled(false);
	}

	UIElement::~UIElement()
	{
		for(auto i = children_.Begin(); i!= children_.End(); ++i)
		{
			if(i->Refs() > 1)
			{
				(*i)->Detach();
			}
		}
	}

	void UIElement::RegisterObject(Context *context)
	{
		context->RegisterFactory<UIElement>(UI_CATEGORY);
		//todo
	}

	void UIElement::ApplyAttributes()
	{
		colorGradient_ = false;
		derivedColorDirty_ = true;
		for(unsigned i=1; i< MAX_UIELEMENT_CORNESS; ++i)
		{
			if(colors_[i] != colors_[0])
				colorGradient_ = true;
		}
	}

	bool UIElement::LoadXML(const XMLElement &source)
	{
		return LoadXML(source, nullptr);
	}

	bool UIElement::LoadXML(const XMLElement &source, XMLFile *styleFile)
	{
		String styleName = source.GetAttribute("style");
		if(styleFile)
		{
			if(styleName.Empty())
				styleName = GetTypeName();

			SetStyle(styleName, styleFile);
		}
		else if(!styleName.Empty() && styleName != appliedStyle_)
		{
			styleFile = GetDefaultStyle();
			if(styleFile)
			{
				String appliedStyle(appliedStyle_);
				SetStyle(styleName, styleFile);
				appliedStyle_ = appliedStyle;
			}
		}

		DisableLayoutUpdate();

		if(!Animatable::LoadXML(source))
			return false;

		unsigned nextInternalChild = 0;

		XMLElement childElem = source.GetChild("element");
		while (childElem)
		{
			bool internalElem = childElem.GetBool("internal");
			String typeName = childElem.GetAttribute("type");
			if(typeName.Empty())
				typeName = "UIElement";
			unsigned index = childElem.HasAttribute("index") ? childElem.GetUInt("index") : M_MAX_UNSIGNED;
			UIElement* child = nullptr;

			if(!internalElem)
			{
				child = CreateChild(typeName, String::EMPTY, index);
			}
			else
			{
				for(unsigned i=nextInternalChild; i < children_.Size(); ++i)
				{
					if(children_[i]->IsInternal() && children_[i]->GetTypeName() == typeName)
					{
						child = children_[i];
						nextInternalChild = i + 1;
						break;
					}
				}

				if(!child)
					URHO3D_LOGWARNING("Could not find matching internal child element of type " + typeName + " in " + GetTypeName());

			}

			if(child)
			{
				if(!styleFile)
					styleFile = GetDefaultStyle();
				if(!child->LoadXML(childElem, styleFile))
					return false;
			}

			childElem = childElem.GetNext("element");
		}

		ApplyAttributes();

		EnableLayoutUpdate();
		UpdateLayout();
		return true;
	}



	void UIElement::GetChildrenRecursive(Urho3D::PODVector<Urho3D::UIElement *> &dest) const
	{
		for(auto i = children_.Begin(); i != children_.End(); ++i)
		{
			UIElement* element = *i;
			dest.Push(element);
			if(!element->children_.Empty())
				element->GetChildrenRecursive(dest);
		}
	}

	void UIElement::GetChildrenWithTagRecursive(PODVector<UIElement *> &dest, const String &tag) const
	{
		for(auto i = children_.Begin(); i != children_.End(); ++i)
		{
			UIElement* element = *i;
			if(element->HasTag(tag))
				dest.Push(element);
			if(!element->children_.Empty())
				element->GetChildrenWithTagRecursive(dest, tag);
		}
	}

	void UIElement::ApplyStyleRecursive(UIElement *element)
	{
		if(!element->appliedStyle_.Empty() && element->appliedStyleFile_.Get() != element->GetDefaultStyle())
		{
			element->SetStyle(element->appliedStyle_);
			for(auto i = element->children_.Begin(); i!= element->children_.End(); ++i)
			{
				element->ApplyStyleRecursive(*i);
			}
		}
	}

	XMLFile *UIElement::GetDefaultStyle(bool recursiveUp) const
	{
		if(recursiveUp)
		{
			const UIElement* element = this;
			while(element)
			{
				if(element->defaultStyle_)
					return element->defaultStyle_;
				element = element->parent_;
			}
		}
		else
		{
			return defaultStyle_;
		}
	}

	bool UIElement::SetStyle(const String &styleName, XMLFile *file)
	{
		todo:
	}

	UIElement *UIElement::CreateChild(StringHash type, const String &name, unsigned int index)
	{
		SharedPtr<UIElement> newElement = DynamicCast<UIElement>(context_->CreateObject(type));
		if(!newElement)
		{
			URHO3D_LOGERROR("Could not create unknow UI element type " + type.ToString());
			return nullptr;
		}
		if(!name.Empty())
			newElement->SetName(name);

		InsertChild(index, newElement);
		return newElement;
	}

	void UIElement::InsertChild(unsigned index, UIElement *element)
	{
		if(!element || element == this || element->parent_ == this)
			return;

		UIElement* parent = parent_;
		while(parent)
		{
			if(parent == element)
				return;
			parent = parent->parent_;
		}

		if(index >= children_.Size())
			children_.Push(SharedPtr<UIElement>(element));
		else
			children_.Insert(children_.Begin() + index, SharedPtr<UIElement>(element));

		element->Remove();

		if(sortChildren_)
			sortOrderDirty_ = true;

		element->parent_ = this;
		element->MarkDirty();

		ApplyStyleRecursive(element);

		VerifyChildAlignment();
		UpdateLayout();

		UIElement* root = GetRoot();
		//todo
	}

	void UIElement::Remove()
	{
		if(parent_)
			parent_->RemoveChild(this);
	}

	void UIElement::RemoveChild(UIElement *element, unsigned int index)
	{
		for(unsigned i=index; i<children_.Size(); ++i)
		{
			if(children_[i] == element)
			{
				UIElement* sender = Refs() > 0 ? GetElementEventSender() : nullptr;
				if(sender)
				{
					using namespace ElementRemoved;

					VariantMap& eventData = GetEventDataMap();
					eventData[P_ROOT] = GetRoot();
					eventData[P_PARENT] = this;
					eventData[P_ELEMENT] = element;

					sender->SendEvent(E_ELEMENTREMOVED, eventData);
				}

				element->Detach();
				children_.Erase(i);
				UpdateLayout();
				return;
			}
		}
	}

	UIElement *UIElement::GetElementEventSender() const
	{
		auto* element = const_cast<UIElement*>(this);
		if(elementEventSender_)
			return element;

		while (element->parent_)
		{
			element = element->parent_;
			if(element->elementEventSender_)
				return element;
		}

		// return root
		return element;
	}

	void UIElement::Detach()
	{
		parent_ = nullptr;
		MarkDirty();
	}

	void UIElement::MarkDirty()
	{
		positionDirty_ = true;
		opacityDirty_ = true;
		derivedColorDirty_ = true;

		for(auto i = children_.Begin(); i<children_.End(); ++i)
			(*i)->MarkDirty();
	}

	void UIElement::UpdateLayout()
	{
		if(layoutNestingLevel_)
			return;

		DisableLayoutUpdate();

		PODVector<int> positions;
		PODVector<int> sizes;
		PODVector<int> minSizes;
		PODVector<int> maxSizes;
		PODVector<float> flexScales;

		int baseIndentWidth = GetIndentWidth();

		if(layoutMode_ == LM_HORIZONTAL)
		{
			int minChildHeight = 0;

			for(unsigned i=0; i< children_.Size(); ++i)
			{
				if(!children_[i]->IsVisible())
					continue;
				positions.Push(baseIndentWidth);
				auto indent = (unsigned)children_[i]->GetIndentWidth();
				sizes.Push(children_[i]->GetWidth() + indent);
				minSizes.Push(children_[i]->GetEffectiveMinSize().x_ + indent);
				maxSizes.Push(children_[i]->GetMaxWidth() + indent);
				flexScales.Push(children_[i]->GetLayoutFlexScale().x_);
				minChildHeight = Max(minChildHeight, children_[i]->GetEffectiveMinSize().y_);
			}

			CalculateLayout(positions, sizes, minSizes, maxSizes, flexScales, GetWidth(), layoutBorder_.left_, layoutBorder_.right_, layoutSpacing_);
			
			int width = CalculateLayoutParentSize(sizes, layoutBorder_.left_, layoutBorder_.right_, layoutSpacing_);
			int height = Max(GetHeight(), minChildHeight + layoutBorder_.top_ + layoutBorder_.bottom_);
			int minWidth = CalculateLayoutParentSize(minSizes, layoutBorder_.left_, layoutBorder_.right_, layoutSpacing_);
			int minHeight = minChildHeight + layoutBorder_.top_ + layoutBorder_.bottom_;
			layoutMinSize_ = IntVector2(minWidth, minHeight);
			SetSize(width, height);

			width = size_.x_;
			height = size_.y_;

			unsigned j = 0;
			for (int i = 0; i < children_.Size(); ++i)
			{
				if(!children_[i]->IsVisible())
					continue;
				children_[i]->SetPosition(positions[j], GetLayoutChildPosition(children_[i]).y_);
				children_[i]->SetSize(sizes[j], height - layoutBorder_.top_ - layoutBorder_.bottom_);
				++j;
			}
		}
		else if(layoutMode_ == LM_VERTICAL)
		{
			int minChildWidth = 0;
			for(unsigned i=0; i< children_.Size(); ++i)
			{
				if(!children_[i]->IsVisible())
					continue;
				positions.Push(0);
				sizes.Push(children_[i]->GetHeight());
				minSizes.Push(children_[i]->GetEffectiveMinSize().y_);
				maxSizes.Push(children_[i]->GetMaxHeight());
				flexScales.Push(children_[i]->GetLayoutFlexScale().y_);
				minChildWidth = Max(minChildWidth, children_[i]->GetEffectiveMinSize().x_ + children_[i]->GetIndentWidth());
			}

			CalculateLayout(positions, sizes, minSizes, maxSizes, flexScales, GetHeight(), layoutBorder_.top_, layoutBorder_.bottom_, layoutSpacing_);

			int height = CalculateLayoutParentSize(sizes, layoutBorder_.top_, layoutBorder_.bottom_, layoutSpacing_);
			int width = Max(GetWidth(), minChildWidth + layoutBorder_.left_ + layoutBorder_.right_);
			int minHeight = CalculateLayoutParentSize(minSizes, layoutBorder_.top_, layoutBorder_.bottom_, layoutSpacing_);
			int minWidth = minChildWidth + layoutBorder_.left_ + layoutBorder_.right_;
			layoutMinSize_ = IntVector2(minWidth, minHeight);
			SetSize(width, height);
			width = size_.x_;
			height = size_.y_;

			unsigned k = 0;
			for(unsigned i=0; i< children_.Size(); ++i)
			{
				if(!children_[i]->IsVisible())
					continue;
				// NOTE 如果是纵向的排列，X 值应该是相同的
				children_[i]->SetPosition(GetLayoutChildPosition(children_[i]).x_ + baseIndentWidth, positions[k]);
				children_[i]->SetSize(width - layoutBorder_.left_ - layoutBorder_.right_, sizes[k]);
				++k;
			}
		}
		else
		{
			for (int i = 0; i < children_.Size(); ++i)
			{
				if(children_[i]->GetEnableAnchor())
					children_[i]->UpdateAnchoring();
			}
		}

		using namespace LayoutUpdated;
		//todo
	}

	void UIElement::CalculateLayout(PODVector<int> &positions, PODVector<int> &sizes, const PODVector<int> &minSizes,
	                                const PODVector<int> &maxSizes, const PODVector<float> &flexScales, int targetSize,
	                                int begin, int end, int spacing)
	{
		unsigned numChildren = sizes.Size();
		if(!numChildren)
			return;

		//Note 为啥这么计算？ begin 和 end 都是 padding
		int targetTotalSize = targetSize - begin - end - (numChildren - 1) * spacing;
		if(targetTotalSize < 0)
			targetTotalSize = 0;

		int targetChildSize = targetTotalSize / numChildren;
		int remainder = targetTotalSize % numChildren;
		float add = (float)remainder / numChildren;
		float acc = 0.0f;

		// Initial pass
		for(unsigned i=0; i<numChildren; ++i)
		{
			auto targetSize = (int)(targetChildSize * flexScales[i]);
			if(remainder)
			{
				acc += add;
				// TODO 这里怎么理解？
				if(acc >= 0.5f)
				{
					acc -= 1.0f;
					++targetSize;
					--remainder;
				}
			}
			sizes[i] = Clamp(targetSize, minSizes[i], maxSizes[i]);
		}

		// Error correction pass
		for(;;)
		{
			int actualTotalSize = 0;
			for(unsigned i=0; i< numChildren; ++i)
				actualTotalSize += sizes[i];

			//NOTE  最终这两个值会相等
			int error = targetTotalSize - actualTotalSize;
			if(!error)
				break;

			PODVector<unsigned > resizable;
			for(unsigned i=0; i<numChildren; ++i)
			{
				if(error < 0 && sizes[i] > minSizes[i])
					resizable.Push(i);
				else if(error > 0 && sizes[i] < maxSizes[i])
					resizable.Push(i);
			}

			if(resizable.Empty())
				break;

			int numResizable = resizable.Size();
			int errorPerChild = error / numResizable;
			remainder = abs(error) % numResizable;
			add = (float)remainder / numResizable;
			acc = 0.0f;

			for(int i=0; i< numResizable; ++i)
			{
				unsigned index = resizable[i];
				int targetSize = sizes[index] + errorPerChild;
				if(remainder)
				{
					acc += add;
					if(acc >= 0.5f)
					{
						acc -= 1.0f;
						targetSize = error < 0 ? targetSize - 1 : targetSize + 1;
						--remainder;
					}
				}

				sizes[i] = Clamp(targetSize, minSizes[index], maxSizes[index]);
			}
		}

		layoutElementMaxSize_ = 0;
		int position = begin;
		for (int i = 0; i < numChildren; ++i)
		{
			positions[i] = position;
			position += sizes[i] + spacing;
			if(sizes[i] > layoutElementMaxSize_)
				layoutElementMaxSize_ = sizes[i];
		}
	}

	int UIElement::CalculateLayoutParentSize(const PODVector<int> &sizes, int begin, int end, int spacing)
	{
		int width = begin + end;
		if(sizes.Empty())
			return width;

		for (int i = 0; i < sizes.Size(); ++i)
		{
			if(sizes[i] == M_MAX_UNSIGNED)
				return M_MAX_UNSIGNED;

			width += sizes[i] + spacing;
		}
		return width - spacing;
	}

	IntVector2 UIElement::GetLayoutChildPosition(UIElement *child)
	{
		IntVector2 ret(IntVector2::ZERO);

		auto ha = child->GetHorizontalAlignment();
		switch(ha)
		{
			case HA_LEFT:
				ret.x_ = layoutBorder_.left_;
				break;
			case HA_RIGHT:
				ret.x_ = -layoutBorder_.right_;
				break;
			default:
				break;
		}

		VerticalAlignment va = child->GetVerticalAlignment();
		switch(va)
		{
			case VA_TOP:
				ret.y_ = layoutBorder_.top_;
				break;
			case VA_BOTTOM:
				ret.y_ = -layoutBorder_.bottom_;
				break;
			default:
				break;
		}
		return ret;
	}

	void UIElement::SetPosition(int x, int y)
	{
		SetPosition(IntVector2(x, y));
	}

	void UIElement::SetPosition(const IntVector2 &position)
	{
		if(position != position_)
		{
			position_ = position;
			OnPositionSet(position);
			MarkDirty();

			using namespace Positioned;
			//todo
		}
	}

	void UIElement::SetSize(int width, int height)
	{
		SetSize(IntVector2(width, height));
	}

	void UIElement::UpdateAnchoring()
	{
		if(parent_ && enableAnchor_)
		{
			IntVector2 newSizes;
			newSizes.x_ = (int)(parent_->size_.x_ * Clamp(anchorMax_.x_ - anchorMin_.x_, 0.0f, 1.0f)) + maxOffset_.x_ - minOffset_.x_;
			newSizes.y_ = (int)(parent_->size_.y_ * Clamp(anchorMax_.y_ - anchorMin_.y_, 0.0f, 1.0f)) + maxOffset_.y_ - minOffset_.y_;

			//TODO 这是干啥， position 和 offset 应该是两个不同的概念呀
			if(position_ != minOffset_)
				SetPosition(minOffset_);
			if(size_ != newSizes)
				SetSize(newSizes);
		}
	}

	void UIElement::SetSize(const IntVector2 &size)
	{
		++resizeNestingLevel_;

		IntVector2 oldSize = size_;
		IntVector2 validatedSize;
		IntVector2 effectiveMinSize = GetEffectiveMinSize();
		validatedSize.x_ = Clamp(size.x_, effectiveMinSize.x_, maxSize_.x_);
		validatedSize.y_ = Clamp(size.y_, effectiveMinSize.y_, maxSize_.y_);

		if(validatedSize != size_)
		{
			size_ = validatedSize;
			if(resizeNestingLevel_ == 1)
			{
				if(parent_)
					parent_->UpdateLayout();

				IntVector2 delta = size_ - oldSize;
				MarkDirty();
				OnResize(size_, delta);
				UpdateLayout();

				//todo event

			}
		}

		--resizeNestingLevel_;
	}

	IntVector2 UIElement::GetEffectiveMinSize() const
	{
		if(IsFixedSize() || layoutMode_ == LM_FREE || layoutMinSize_ == IntVector2::ZERO)
		{
			return minSize_;
		}
		else
		{
			return IntVector2(Max(minSize_.x_, layoutMinSize_.x_), Max(minSize_.y_, layoutMinSize_.y_));
		}
	}

	const IntVector2 &UIElement::GetScreenPosition() const
	{
		if(positionDirty_)
		{
			IntVector2 pos = position_;
			const UIElement* parent = parent_;

			if(parent)
			{
				const IntVector2& parentScreenPos = parent->GetScreenPosition();

				//todo, 这里没有理解，anchorMin 是相对于父亲节点的左上角而言的，pivot 是控制对齐的，影响 pos，如果 pivot 是 1，1，那么表示 pos 是以右下角为基准计算了
				//所以，转换到屏幕坐标的时候需要减去 pivot 的偏移
				pos.x_ += parentScreenPos.x_ + (int)Lerp(0.0f, (float)parent->size_.x_, anchorMin_.x_);
				pos.y_ += parentScreenPos.y_ + (int)Lerp(0.0f, (float)parent->size_.y_, anchorMin_.y_);
				pos.x_ -= (int)(size_.x_ * pivot_.x_);
				pos.y_ -= (int)(size_.y_ * pivot_.y_);

				pos += parent->childOffset_;
			}

			screenPosition_ = pos;
			positionDirty_ = false;
		}

		return screenPosition_;
	}

	float UIElement::GetDerivedOpacity() const
	{
		if(!useDerivedOpacity_)
			return opacity_;

		if(opacityDirty_)
		{
			derivedOpacity_ = opacity_;
			const UIElement* parent = parent_;

			while(parent)
			{
				derivedOpacity_ *= parent->opacity_;
				parent = parent->parent_;
			}

			opacityDirty_ = false;
		}
		return derivedOpacity_;
	}

	void UIElement::GetBatchesWithOffset(IntVector2 &offset, PODVector<UIBatch> &batches, PODVector<float> &vertexData,
	                                     IntRect currentScissor)
	{
		Vector2 floatOffset((float)offset.x_, (float)offset.y_);
		unsigned initialSize = vertexData.Size();

		GetBatches(batches, vertexData, currentScissor);
		for(unsigned i= initialSize; i < vertexData.Size(); i += 6)
		{
			vertexData[i] += floatOffset.x_;
			vertexData[i + 1] += floatOffset.y_;
		}

		AdjustScissor(currentScissor);
		for(Vector<SharedPtr<UIElement> >::ConstIterator i = children_.Begin(); i != children_.End(); ++i)
		{
			if((*i)->IsVisible())
				(*i)->GetBatchesWithOffset(offset, batches, vertexData, currentScissor);
		}
	}

	void UIElement::GetBatches(PODVector<UIBatch> &batches, PODVector<float> &vertexData, const IntRect &currentScissor)
	{
		hovering_ = false;
	}

	void UIElement::Update(float timeStep)
	{
	}

	void UIElement::AdjustScissor(IntRect &currentScissor)
	{
		if(clipChildren_)
		{
			IntVector2 screenPos = GetScreenPosition();
			currentScissor.left_ = Max(currentScissor.left_, screenPos.x_ + clipBorder_.left_);
			currentScissor.top_ = Max(currentScissor.top_, screenPos.y_ + clipBorder_.top_);
			currentScissor.right_ = Min(currentScissor.right_, screenPos.x_ + size_.x_ - clipBorder_.right_);
			currentScissor.bottom_ = Min(currentScissor.bottom_, screenPos.y_ + size_.y_ - clipBorder_.bottom_);

			if(currentScissor.right_ < currentScissor.left_)
				currentScissor.right_ = currentScissor.left_;
			if(currentScissor.bottom_ < currentScissor.top_)
				currentScissor.bottom_ = currentScissor.top_;
		}
	}

	void UIElement::SortChildren()
	{
		//todo
	}

	bool UIElement::IsWithinScissor(const IntRect &currentScissor)
	{
		//todo
	}

}

