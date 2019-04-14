//
// Created by liuhao on 2019/2/23.
//

#ifndef URHO3DCOPY_UIELEMENT_HPP
#define URHO3DCOPY_UIELEMENT_HPP

#include "../Container/FlagSet.hpp"
#include "../Scene/Animatable.h"
#include "../Resource/XMLFile.h"
#include "UIBatch.hpp"
#include "../Math/Rect.h"

namespace Urho3D
{
	enum HorizontalAlignment
	{
		HA_LEFT = 0,
		HA_CENTER,
		HA_RIGHT,
		HA_CUSTOM
	};

	enum VerticalAlignment
	{
		VA_TOP = 0,
		VA_CENTER,
		VA_BOTTOM,
		VA_CUSTOM
	};

	enum Corner
	{
		C_TOPLEFT = 0,
		C_TOPRIGHT,
		C_BOTTOMLEFT,
		C_BOTTOMRIGHT,
		MAX_UIELEMENT_CORNESS
	};

	enum Orientation
	{
		O_HORIZONTAL = 0,
		O_VERTICAL
	};

	enum FocusMode
	{
		/// Is not focusable and does not affect existing focus.
		FM_NOTFOCUSABLE = 0,
		/// Resets focus when clicked.
		FM_RESETFOCUS,
		/// Is focusable
		FM_FOCUSABLE,
		FM_FOCUSABLE_DEFOCUSABLE
	};

	enum LayoutMode
	{
		LM_FREE = 0,
		LM_HORIZONTAL,
		LM_VERTICAL
	};

	enum TraversalMode
	{
		TM_BREADTH_FIRST = 0,
		TM_DEPTH_FIRST
	};

	enum DragAndDropMode : unsigned
	{
		DD_DISABLED = 0x0,
		DD_SOURCE = 0x1,
		DD_TARGET = 0x2,
		DD_SOURCE_AND_TARGET = 0x3
	};

	URHO3D_FLAGSET(DragAndDropMode, DragAndDropModeFlags);

	class Cursor;
	class ResourceCache;
	class Texture2D;

	class UIElement : public Animatable
	{
		URHO3D_OBJECT(UIElement, Animatable);

	public:
		explicit UIElement(Context* context);
		~UIElement() override;
		static void RegisterObject(Context* context);

		void ApplyAttributes() override;
		bool LoadXML(const XMLElement& source) override;
		virtual bool LoadXML(const XMLElement& source, XMLFile* styleFile);
		virtual UIElement* LoadChildXML(const XMLElement& childElem, XMLFile* styleFile);
		bool SaveXML(XMLElement& dest) const override;

		virtual void Update(float timeStep);
		virtual bool IsWithinScissor(const IntRect& currentScissor);
		virtual const IntVector2& GetScreenPosition() const;
		virtual void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor);
		virtual void GetDebugDrawBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor);
		virtual void OnHover(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers, Cursor* cursor);
		virtual void OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers, Cursor* cursor)
		{}
		virtual void OnClickEnd(const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers,
				Cursor* cursor, UIElement* beginElement)
		{}

		virtual void OnDoubleClick(const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers,
		                        Cursor* cursor)
		{}


		virtual void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers,
		                           Cursor* cursor)
		{}

		virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, const IntVector2& deltaPos, int buttons, int qualifiers,
		                         Cursor* cursor)
		{}

		virtual void OnDragEnd(const IntVector2& position, const IntVector2& screenPosition, int dragButton, Cursor* cursor)
		{}

		virtual void OnDragCancel(const IntVector2& position, const IntVector2& screenPosition, int dragButton, int cancelButton, Cursor* cursor)
		{}

		virtual bool OnDragDropTest(UIElement* source);
		virtual bool OnDragDropFinish(UIElement* source);
		virtual void OnWheel(int delta, MouseButtonFlags buttons, QualifierFlags qualifiers) {}
		virtual void OnKey(Key key, MouseButtonFlags buttons, QualifierFlags qualifiers) {}
		virtual void OnTextInput(const String& text);

		virtual void OnResize(const IntVector2& newSize, const IntVector2& delta) {}
		virtual void OnPositionSet(const IntVector2& newPosition) {}
		virtual void OnSetEditable() {}
		virtual void OnIndentSet() {}

		virtual IntVector2 ScreenToElement(const IntVector2& screenPosition);
		virtual IntVector2 ElementToScreen(const IntVector2& position);

		virtual bool IsWheelHandler() const
		{
			return false;
		}

		bool LoadXML(Deserializer& source);
		bool SaveXML(Serializer& dest, const String& indentation = "\t") const;
		bool FilterAttributes(XMLElement& dest) const;

		void SetName(const String& name);
		void SetPosition(const IntVector2& position);
		void SetPosition(int x, int y);
		void SetSize(const IntVector2& size);
		void SetSize(int width, int height);
		void SetWidth(int width);
		void SetHeight(int height);
		void SetMinSize(const IntVector2& minSize);
		void SetMinSize(int width, int height);
		void SetMinWidth(int width);
		void SetMinHeight(int height);
		void SetMaxSize(const IntVector2& maxSize);
		void SetMaxSize(int width, int height);
		void SetMaxWidth(int width);
		void SetMaxHeight(int height);
		void SetFixedSize(const IntVector2& size);
		void SetFixedSize(int width, int height);
		void SetFixedWidth(int width);
		void SetFixedHeight(int height);

		void SetAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign);
		void SetHorizontalAlignment(HorizontalAlignment align);
		void SetVerticalAlignment(VerticalAlignment align);

		void SetEnableAnchor(bool enable);
		void SetMinAnchor(const Vector2& anchor);
		void SetMinAnchor(float x, float y);
		void SetMaxAnchor(const Vector2& anchor);
		void SetMaxAnchor(float x, float y);
		void SetMinOffset(const IntVector2& offset);
		void SetMaxOffset(const IntVector2& offset);
		void SetPivot(const Vector2& pivot);
		void SetPivot(float x, float y);
		void SetClipBorder(const IntRect& rect);
		void SetColor(const Color& color);
		void SetColor(Corner corner, const Color& color);
		void SetPriority(int priority);
		void SetOpacity(float opacity);
		void SetBringToFont(bool enable);
		void SetBringToBack(bool enable);
		void SetClipChildren(bool enable);
		void SetSortChildren(bool enable);
		void SetUseDerivedOpacity(bool enable);
		void SetEnabled(bool enable);
		void SetDeepEnabled(bool enable);
		void ResetDeepEnabled();
		void SetEnableRecursive(bool enable);
		void SetEditable(bool enable);
		void SetFocus(bool enable);
		void SetSelected(bool enable);
		void SetVisible(bool enable);
		void SetFocusMode(FocusMode mode);
		void SetDragDropMode(DragAndDropModeFlags mode);

		bool SetStyle(const String& styleName, XMLFile* file = nullptr);
		bool SetStyle(const XMLElement& element);
		bool SetStyleAuto(XMLFile* file = nullptr);
		void SetDefaultStyle(XMLFile* style);
		void SetLayout(LayoutMode mode, int spacing = 0, const IntRect& border = IntRect::ZERO);
		void SetLayoutMode(LayoutMode mode);
		void SetLayoutSpacing(int spacing);
		void SetLayoutBorder(const IntRect& border);
		void SetLayoutFlexScale(const Vector2& scale);
		void SetIndent(int indent);
		void SetIndentSpacing(int indentSpacing);
		void UpdateLayout();
		void DisableLayoutUpdate();
		void EnableLayoutUpdate();
		void BringToFront();

		UIElement* CreateChild(StringHash type, const String& name = String::EMPTY, unsigned index = M_MAX_UNSIGNED);
		void AddChild(UIElement* element);
		void InsertChild(unsigned index, UIElement* element);
		void RemoveChild(UIElement* element, unsigned index = 0);
		void RemoveChildAtIndex(unsigned index);
		void RemoveAllChildren();
		void Remove();
		unsigned FindChild(UIElement* element) const;
		void SetParent(UIElement* parent, unsigned index = M_MAX_UNSIGNED);
		void SetVar(StringHash key, const VariantMap& value);
		void SetInternal(bool enable);
		void SetTraversalMode(TraversalMode traversalMode);
		void SetElementEventSender(bool flag);

		void SetTags(const StringVector& tags);
		void AddTag(const String& tag);
		void AddTags(const String& tags, char seperator = ';');
		void AddTags(const StringVector& tags);
		bool RemoveTag(const String& tag);
		void RemoveAllTags();

		template <class T> T* CreateChild(const String& name = String::EMPTY, unsigned index = M_MAX_UNSIGNED);
		template <class T> T* GetChildStaticCast(unsigned index) const;
		template <class T> T* GetChildStaticCast(const String& name, bool recursive = false) const;
		template <class T> T* GetChildStaticCast(const StringHash& key, const Variant& value = Variant::EMPTY, bool recursive = false) const;
		template <class T> T* GetChildDynamicCast(unsigned index) const;
		template <class T> T* GetChildDynamicCast(const String& name, bool recursive = false) const;
		template <class T> T* GetChildDynamicCast(const StringHash& key, const Variant& value = Variant::EMPTY, bool recursive = false) const;

		const String& GetName() const { return name_;}

		const IntVector2& GetPosition() const { return position_; }

		const IntVector2& GetSize() const { return size_; }

		int GetWidth() const { return size_.x_; }
		int GetHeight() const { return size_.y_; }

		const IntVector2& GetMinSize() const { return minSize_; }

		int GetMinWidth() const { return minSize_.x_; }
		int GetMinHeight() const { return minSize_.y_; }

		const IntVector2& GetMaxSize() const { return maxSize_; }
		int GetMaxWidth() const { return maxSize_.x_; }
		int GetMaxHeight() const { return maxSize_.y_; }

		bool IsFixedSize() const { return minSize_ == maxSize_; }
		bool IsFixedWidth() const { return minSize_.x_ == maxSize_.x_;}
		bool IsFixedHeight() const { return minSize_.y_ == maxSize_.y_; }

		const IntVector2& GetChildOffset() const { return childOffset_; }

		HorizontalAlignment GetHorizontalAlignment() const;
		VerticalAlignment GetVerticalAlignment() const;

		bool GetEnableAnchor() const { return enableAnchor_; }
		const Vector2& GetMinAnchor() const { return anchorMin_; }
		const Vector2& GetMaxAnchor() const { return anchorMax_; }

		const IntVector2& GetMinOffset() const { return minOffset_; }
		const IntVector2& GetMaxOffset() const { return maxOffset_; }

		const Vector2& GetPivot() const { return pivot_; }
		const IntRect& GetClipBorder() const { return clipBorder_; }

		const Color& GetColor(Corner corner) const { return colors_[corner]; }

		int GetPriority() const { return priority_; }
		float GetOpacity() const { return opacity_; }
		float GetDerivedOpacity() const;
		bool GetBringToFront() const { return bringToFront_; }
		bool GetBringToBack() const { return bringToBack_; }
		//todo

		bool IsEnabled() const { return enabled_; }
		bool IsEnabledSelf() const { return enabledPrev_; }
		bool IsEditable() const { return editable_; }
		bool IsSelected() const { return selected_; }


		bool IsVisible() const { return visible_; }
		bool IsVisibleEffective() const;
		bool IsHovering() const { return hovering_; }
		bool IsInternal() const { return internal_; }
		bool HasColorGradient() const { return colorGradient_; }

		FocusMode GetFocusMode() const { return focusMode_; }

		DragAndDropModeFlags GetDragDropMode() const { return dragDropMode_; }
		const String& GetAppliedStyle() const;

		XMLFile* GetDefaultStyle(bool recursiveUp = true) const;
		LayoutMode GetLayoutMode() const { return layoutMode_; }
		int GetLayoutSpacing() const { return layoutSpacing_; }
		const IntRect& GetLayoutBorder() const { return layoutBorder_; }
		const Vector2& GetLayoutFlexScale() const { return layoutFlexScale_; }

		unsigned GetNumChildren(bool recursive = false) const;
		UIElement* GetChild(unsigned index) const;
		UIElement* GetChild(const String& name, bool recursive = false) const;
		UIElement* GetChild(const StringHash& key, const Variant& value = Variant::EMPTY, bool recursive = false) const;

		const Vector<SharedPtr<UIElement>>& GetChildren() const { return children_; }

		void GetChildren(PODVector<UIElement*>& dest, bool recursive = false) const;

		UIElement* GetParent() const { return parent_; }
		UIElement* GetRoot() const;

		const Color& GetDerivedColor() const;
		const Variant& GetVar(const StringHash& key) const;

		const VariantMap& GetVars() const { return vars_; }

		bool HasTag(const String& tag) const;
		const StringVector& GetTags() const { return tags_; }
		void GetChildrenWithTag(PODVector<UIElement*>& dest, const String& tag, bool recursive = false) const;
		PODVector<UIElement*> GetChildrenWithTag(const String& tag, bool recursive = false) const;

		int GetDragButtonCombo() const { return dragButtonCombo_; }
		unsigned GetDragButtonCount() const { return dragButtonCount_; }

		bool IsInside(IntVector2 position, bool isScreen);
		bool IsInsideCombined(IntVector2 position, bool isScreen);
		IntRect GetCombinedScreenRect();

		void SortChildren();

		int GetLayoutElementMaxSize() const { return layoutElementMaxSize_; }

		int GetIndent() const { return indent_; }
		int GetIndentSpacing() const { return indentSpacing_; }
		int GetIndentWidth() const { return indent_ * indentSpacing_; }

		void SetChildOffset(const IntVector2& offset);
		void SetHovering(bool enable);
		void AdjustScissor(IntRect& currentScissor);
		void GetBatchesWithOffset(IntVector2& offset, PODVector<UIBatch>& batches,
				PODVector<float>& vertexData, IntRect currentScissor);

		const Color& GetColorAttr() const { return colors_[0]; }

		TraversalMode GetTraversalMode() const { return traversalMode_; }

		bool IsElementEventSender() const { return elementEventSender_; }
		UIElement* GetElementEventSender() const;
		IntVector2 GetEffectiveMinSize() const;
		void SetRenderTexture(Texture2D* texture);


		//todo

		//todo

	protected:
		void OnAttributeAnimationAdded() override;
		void OnAttributeAnimationRemoved() override;
		Animatable* FindAttributeAnimationTarget(const String& name, String& outName) override;
		void MarkDirty();
		bool RemoveChildXML(XMLElement& parent, const String& name) const;
		bool RemoveChildXML(XMLElement& parent, const String& name, const String& value) const;
		bool FilterUIStyleAttributes(XMLElement& dest, const XMLElement& styleelem) const;
		virtual bool FilterImplicitAttributes(XMLElement& dest) const;
		void UpdateAnchoring();

		String name_;
		Vector<SharedPtr<UIElement> > children_;
		UIElement* parent_{};
		IntRect clipBorder_;
		Color colors_[MAX_UIELEMENT_CORNESS];
		VariantMap vars_;
		int priority_{};
		bool bringToFront_{};
		bool bringToBack_{};
		bool clipChildren_{};
		bool sortChildren_{true};
		bool useDerivedOpacity_{true};
		bool enabled_{};
		bool enabledPrev_{};
		bool editable_{true};
		bool selected_{};
		bool visible_{true};
		bool hovering_{};
		bool internal_{};
		FocusMode focusMode_{FM_NOTFOCUSABLE};
		DragAndDropModeFlags dragDropMode_{DD_DISABLED};
		LayoutMode layoutMode_{LM_FREE};
		int layoutSpacing_{};
		IntRect layoutBorder_{};
		Vector2 layoutFlexScale_{Vector2::ONE};
		unsigned resizeNestingLevel_{};
		unsigned layoutNestingLevel_{};
		int layoutElementMaxSize_{};
		int indent_{};
		int indentSpacing_{16};
		IntVector2 position_{};
		mutable IntVector2 screenPosition_;
		mutable bool positionDirty_{true};
		String appliedStyle_;
		int dragButtonCombo_{};
		unsigned dragButtonCount_{};


	private:
		void GetChildrenRecursive(PODVector<UIElement*>& dest) const;
		void GetChildrenWithTagRecursive(PODVector<UIElement*>& dest, const String& tag) const;
		void ApplyStyleRecursive(UIElement* element);
		int CalculateLayoutParentSize(const PODVector<int>& sizes, int begin, int end, int spacing);
		void CalculateLayout(PODVector<int>& positions, PODVector<int>& sizes, const PODVector<int>& minSizes,
				const PODVector<int>& maxSizes, const PODVector<float>& flexScales, int targetSize,
				int begin, int end, int spacing);

		IntVector2 GetLayoutChildPosition(UIElement* child);
		void Detach();
		void VerifyChildAlignment();
		void HandlePostUpdate(StringHash eventType, VariantMap& eventData);

		IntVector2 size_;
		IntVector2 minSize_;
		IntVector2 maxSize_{M_MAX_INT, M_MAX_INT};
		IntVector2 childOffset_;
		IntVector2 layoutMinSize_;
		IntVector2 minOffset_;
		IntVector2 maxOffset_;

		bool enableAnchor_{};
		bool pivotSet_{};
		Vector2 anchorMin_;
		Vector2 anchorMax_;
		Vector2 pivot_;
		float opacity_{1.0f};
		mutable float derivedOpacity_{};
		mutable Color derivedColor_;
		mutable bool opacityDirty_{true};
		mutable bool derivedColorDirty_{true};
		bool sortOrderDirty_{};
		bool colorGradient_{};
		SharedPtr<XMLFile> defaultStyle_;
		WeakPtr<XMLFile> appliedStyleFile_;
		TraversalMode traversalMode_{TM_BREADTH_FIRST};
		bool elementEventSender_{};
		static XPathQuery styleXPathQuery_;
		StringVector tags_;
	};
}



#endif //URHO3DCOPY_UIELEMENT_HPP
