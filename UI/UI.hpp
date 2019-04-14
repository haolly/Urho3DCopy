//
// Created by liuhao on 2019-04-08.
//

#ifndef URHO3DCOPY_UI_HPP
#define URHO3DCOPY_UI_HPP

#include "../Core/Object.h"
#include "../IO/Deserializer.h"
#include "../IO/Serializer.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/VertexBuffer.h"

namespace Urho3D
{
	class Cursor;
	class Graphics;
	class ResourceCache;
	class Timer;
	class UIBatch;
	class UIElement;
	class XMLElement;
	class XMLFile;
	class RenderSurface;
	class UIComponent;

	class UI : public Object
	{
		URHO3D_OBJECT(UI, Object);
	public:
		explicit UI(Context* context);
		~UI() override;

		void SetCursor(Cursor* cursor);

		void SetFocusElement(UIElement* element, bool byKey = false);
		bool SetModalElement(UIElement* modalElement, bool enable);
		void Clear();

		void Update(float timeStep);

		void RenderUpdate();

		void Render(bool renderUICommand = false);
		void DebugDraw(UIElement* element);
		SharedPtr<UIElement> LoadLayout(Deserializer& source, XMLFile* styleFile = nullptr);
		SharedPtr<UIElement> LoadLayout(XMLFile* file, XMLFile* styleFile = nullptr);
		bool SaveLayout(Serializer& dest, UIElement* element);
		//todo

	private:
		struct RenderToTextureData
		{
			WeakPtr<UIElement> rootElement_;
			SharedPtr<Texture2D> texture_;
			PODVector<UIBatch> batches_;
			PODVector<float> vertexData_;
			SharedPtr<VertexBuffer> vertexBuffer_;
			PODVector<UIBatch> debugDrawBatches_;
			PODVector<float> debugVertexData_;
			SharedPtr<VertexBuffer> debugVertexBuffer_;
		};

		void Initialize();
		void Update(float timeStep, UIElement* element);
		void SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData);
		void Render(VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd);
		void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, UIElement* element, IntRect currentScissor);
		UIElement* GetElementAt(const IntVector2& position, bool enabledOnly, IntVector2* elementScreenPosition);
		void GetElementAt(UIElement* result, UIElement* current, const IntVector2& position, bool enabledOnly);
		UIElement* GetFocusableElement(UIElement* element);

		//todo

		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
		void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
		void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);


		//todo

		void ResizeRootElement();
		IntVector2 GetEffectiveRootElementSize(bool applyScale = true) const;

		//todo

		WeakPtr<Graphics> graphics_;
		SharedPtr<UIElement> rootElement_;
		SharedPtr<UIElement> rootModalElement_;
		SharedPtr<Cursor> cursor_;
		WeakPtr<UIElement> focusElement_;
		PODVector<UIBatch> batches_;
		PODVector<float> vertexData_;
		PODVector<UIBatch> debugDrawBatches_;
		PODVector<float> debugVertexData_;
		SharedPtr<VertexBuffer> vertexBuffer_;
		SharedPtr<VertexBuffer> debugVertexBuffer_;
		PODVector<UIElement> tempElements_;

		mutable String clipBord_;

		//todo

		bool uiRendered_;
		unsigned nonModalBatchSize_;
		Timer clickTimer_;
		WeakPtr<UIElement> doubleClickElement_;
		IntVector2 doubleClickFirstPos_;
		float maxDoubleClickDist_;
		HashMap<WeakPtr<UIElement>, bool> hoveredElements_;
		HashMap<WeakPtr<UIElement>, DragData*> dragElements_;
		//todo

		float defaultToolTipDelay_;
		int dragBeginDistance_;
		MouseButtonFlags mouseButtons_;
		MouseButtonFlags lastMouseButtons_;
		QualifierFlags qualifiers_;
		int maxFontTextureSize_;
		bool initialized_;
		bool usingTouchInput_;
		//todo

		float uiScale_;
		IntVector2 customSize_;
		HashMap<UIElement*, RenderToTextureData> renderToTexture_;

	};

	void RegisterUILibrary(Context* context);

}



#endif //URHO3DCOPY_UI_HPP
