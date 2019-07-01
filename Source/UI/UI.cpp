//
// Created by liuhao on 2019-04-08.
//

#include "UI.hpp"
#include "UIElement.hpp"
#include "Sprite.hpp"
#include "../Graphics/Graphics.h"
#include "../Core/CoreEvent.h"
#include "../Math/Matrix4.hpp"

namespace Urho3D
{
	const float DEFAULT_DOUBLECLICK_INTERVAL = 0.5F;
	const float DEFAULT_DRAGBEGIN_INTERVAL = 0.5F;


	const char* UI_CATEGORY = "UI";

	UI::UI(Urho3D::Context *context) :
			Object(context),
			rootElement_(new UIElement(context)),
			rootModalElement_(new UIElement(context)),
			//todo

	{
		rootElement_->SetTraversalMode(TM_DEPTH_FIRST);
		rootModalElement_->SetTraversalMode(TM_DEPTH_FIRST);

		RegisterUILibrary(context_);

//		SubscribeToEvent(E_)
		Initialize();
	}

	void UI::Initialize()
	{
		auto* graphics = GetSubsystem<Graphics>();
		if(!graphics || !graphics->IsInitialized())
			return;

//		URHO3D_PROFILE(InitUI);
		graphics_ = graphics;
		UIBatch::posAdjust = Vector3(Graphics::GetPixelUVOffset(), 0.0f);

		ResizeRootElement();

		vertexBuffer_ = new VertexBuffer(context_);
		debugVertexBuffer_ = new VertexBuffer(context_);

		initialized_ = true;

		SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(UI, HandleBeginFrame));
		SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(UI, HandlePostUpdate));
		SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(UI, HandleRenderUpdate));

		URHO3D_LOGINFO("Initialized user interface");
	}

	void UI::HandlePostUpdate(StringHash eventType, VariantMap &eventData)
	{
		using namespace PostUpdate;
		Update(eventData[P_TIMESTEP].GetFloat());
	}

	void UI::ResizeRootElement()
	{
		IntVector2 effectiveSize = GetEffectiveRootElementSize();
		rootElement_->SetSize(effectiveSize);
		rootModalElement_->SetSize(effectiveSize);
	}

	IntVector2 UI::GetEffectiveRootElementSize(bool applyScale) const
	{
		IntVector2 size = graphics_ ? IntVector2(graphics_->GetWidth(), graphics_->GetHeight()) : IntVector2(1024, 768);
		if (customSize_.x_ > 0 && customSize_.y_ > 0)
			size = customSize_;

		if(applyScale)
		{
			size.x_ = RoundToInt((float)size.x_/ uiScale_);
			size.y_ = RoundToInt((float)size.y_/ uiScale_);
		}
		return size;
	}

	void UI::HandleBeginFrame(StringHash eventType, VariantMap &eventData)
	{
		//todo
	}

	void UI::Update(float timeStep)
	{
		assert(rootElement_ && rootModalElement_);

		for(auto it = hoveredElements_.Begin(); it!= hoveredElements_.End(); ++it)
			it->second_ = false;

		auto* input = GetSubsystem<Input>();
		//todo

		Update(timeStep, rootElement_);
		Update(timeStep, rootModalElement_);
	}

	void UI::Update(float timeStep, UIElement *element)
	{
		WeakPtr<UIElement> elementWeak(element);

		element->Update(timeStep);
		if(elementWeak.Expired())
			return;

		const Vector<SharedPtr<UIElement>>& children = element->GetChildren();
		for(unsigned i=0; i< children.Size(); ++i)
			Update(timeStep, children[i]);
	}

	void UI::RenderUpdate()
	{
		assert(rootElement_ && rootModalElement_);

		uiRendered_ = false;

		bool osCursorVisible = GetSubsystem<Input>()->IsMouseVisible();
		batches_.Clear();
		vertexData_.Clear();
		const IntVector2& rootSize = rootElement_->GetSize();
		const IntVector2& rootPos = rootElement_->GetPosition();

		IntRect currentScissor = IntRect(rootPos.x_, rootPos.y_, rootPos.x_ + rootSize.x_, rootPos.y_ + rootSize.y_);
		if(rootElement_->IsVisible())
			GetBatches(batches_, vertexData_, rootElement_, currentScissor);

		nonModalBatchSize_ = batches_.Size();

		GetBatches(batches_, vertexData_, rootModalElement_, currentScissor);

		//todo

		for(auto it = renderToTexture_.Begin(); it != renderToTexture_.End(); )
		{
			RenderToTextureData& data = it->second_;
			if(data.rootElement_.Expired())
			{
				it = renderToTexture_.Erase(it);
				continue;
			}

			if(data.rootElement_->IsEnabled())
			{
				data.batches_.Clear();
				data.vertexData_.Clear();
				UIElement* element = data.rootElement_;
				const IntVector2& size = element->GetSize();
				const IntVector2& pos = element->GetPosition();
				IntRect scissor = IntRect(pos.x_, pos.y_, pos.x_ + size.x_, pos.y_ + size.y_);
				GetBatches(data.batches_, data.vertexData_, element, scissor);

				if(data.batches_.Empty())
				{
					UIBatch batch(element, BLEND_REPLACE, scissor, nullptr, &data.vertexData_);
					batch.SetColor(Color::BLACK);
					batch.AddQuad(scissor.left_, scissor.top_, scissor.Width(), scissor.Height(), 0, 0);
					data.batches_.Push(batch);
				}
			}
			++it;
		}
	}

	// called by Engine and View class
	void UI::Render(bool renderUICommand)
	{
		if(!renderUICommand)
		{
			//todo
		}

		if(renderUICommand || !uiRendered_)
		{
			SetVertexData(vertexBuffer_, vertexData_);
			SetVertexData(debugVertexBuffer_, debugVertexData_);

			if(!renderUICommand)
				graphics_->ResetRenderTargets();

			Render(vertexBuffer_, batches_, 0, nonModalBatchSize_);
			Render(debugVertexBuffer_, debugDrawBatches_, 0, debugDrawBatches_.Size());
			Render(vertexBuffer_, batches_, nonModalBatchSize_, batches_.Size());
		}

		// Render to UIComponent textures
		if(!renderUICommand)
		{
			for(auto& item : renderToTexture_)
			{
				RenderToTextureData& data = item.second_;
				if(data.rootElement_->IsEnabled())
				{
					SetVertexData(data.vertexBuffer_, data.vertexData_);
					SetVertexData(data.debugVertexBuffer_, data.debugVertexData_);

					RenderSurface* surface = data.texture_->GetRenderSurface();
					graphics_->SetDepthStencil(surface->GetLinkedDepthStencil());
					graphics_->SetRenderTarget(0, surface);
					graphics_->SetViewport(IntRect(0, 0, surface->GetWidth(), surface->GetHeight()));
					graphics_->Clear(Urho3D::CLEAR_COLOR);

					Render(data.vertexBuffer_, data.batches_, 0, data.batches_.Size());
					Render(data.debugVertexBuffer_, data.debugDrawBatches_, 0, data.debugDrawBatches_.Size());
					data.debugDrawBatches_.Clear();
					data.debugVertexData_.Clear();
				}
			}

			if(renderToTexture_.Size())
				graphics_->ResetRenderTargets();
		}

		//TODO, WHY not clear normal batches and data
		debugDrawBatches_.Clear();
		debugVertexData_.Clear();

		uiRendered_ = true;
	}

	void UI::GetBatches(PODVector<UIBatch> &batches, PODVector<float> &vertexData, UIElement *element,
	                    IntRect currentScissor)
	{
		element->AdjustScissor(currentScissor);
		if(currentScissor.left_ == currentScissor.right_ || currentScissor.top_ == currentScissor.bottom_)
			return;

		element->SortChildren();
		const auto& children = element->GetChildren();
		if(children.Empty())
			return;

		Vector<SharedPtr<UIElement>>::ConstIterator i = children.Begin();
		if(element->GetTraversalMode() == TM_BREADTH_FIRST)
		{
			Vector<SharedPtr<UIElement>>::ConstIterator j = i;
			while (i != children.End())
			{
				//TODO priority is sibling ???
				int currentPriority = (*i)->GetPriority();
				while ( j != children.End() && (*j)->GetPriority() == currentPriority)
				{
					if((*j)->IsWithinScissor(currentScissor) && (*j) != cursor_)
					{
						(*j)->GetBatches(batches, vertexData, currentScissor);
					}
					++j;
				}

				// recurse into children
				while( i != j)
				{
					if((*i)->IsVisible() && (*i) != cursor_)
					{
						GetBatches(batches, vertexData, *i, currentScissor);
					}
					++i;
				}
			}
		}
		else
		{
			while(i != children.End())
			{
				if((*i) != cursor_)
				{
					if ((*i)->IsWithinScissor(currentScissor))
						(*i)->GetBatches(batches, vertexData, currentScissor);
					if((*i)->IsVisible())
						GetBatches(batches, vertexData, *i, currentScissor);
				}
				++i;
			}
		}
	}

	void UI::SetVertexData(VertexBuffer *dest, const PODVector<float> &vertexData)
	{
		if(vertexData.Empty())
			return;
		unsigned numVertices = vertexData.Size() / UI_VERTEX_SIZE;
		if(dest->GetVertexCount() < numVertices || dest->GetVertexCount() > numVertices * 3)
			dest->SetSize(numVertices, MASK_POSITION | MASK_COLOR | MASK_TEXCOORD1, true);

		dest->SetData(&vertexData[0]);
	}

	void UI::Render(VertexBuffer *buffer, const PODVector<UIBatch> &batches, unsigned batchStart, unsigned batchEnd)
	{
		assert(graphics_ && graphics_->IsInitialized() && !graphics_->IsDeviceLost());

		if(batches.Empty())
			return;

		unsigned alphaFormat = Graphics::GetAlphaFormat();
		RenderSurface* surface = graphics_->GetRenderTarget(0);
		IntVector2 viewSize = graphics_->GetViewport().Size();
		Vector2 invScreenSize(1.0f / (float)viewSize.x_, 1.0f / (float)viewSize.y_);
		//todo, what is this?
		Vector2 scale(2.0f * invScreenSize.x_, -2.0f * invScreenSize.y_);
		//todo, what is this?
		Vector2 offset(-1.0f, 1.0f);

		if(surface)
		{
#ifdef URHO3D_OPENGL
			offset.y_ = -offset.y_;
			scale.y_ = -scale.y_;
#endif
		}

		//todo convert to UI space ?
		Matrix4 projection(Matrix4::IDENTITY);
		projection.m00_ = scale.x_ * uiScale_;
		projection.m03_ = offset.x_;
		projection.m11_ = scale.y_ * uiScale_;
		projection.m13_ = offset.y_;
		projection.m22_ = 1.0f;
		projection.m23_ = 0.0f;
		projection.m33_ = 1.0f;

		graphics_->ClearParameterSources();
		graphics_->SetColorWrite(true);

#ifdef URHO3D_OPENGL
		if(surface)
			graphics_->SetCullMode(CULL_CW);
		else
#endif
			graphics_->SetCullMode(CULL_CCW);

		graphics_->SetDepthTest(CMP_ALWAYS);
		graphics_->SetDepthWrite(false);
		graphics_->SetFillMode(FILL_SOLID);
		graphics_->SetStencilTest(false);
		graphics_->SetVertexBuffer(buffer);

		ShaderVariation* noTextureVS = graphics_->GetShader(VS, "Basic", "VERTEXCOLOR");
		ShaderVariation* diffTextureVS = graphics_->GetShader(VS, "Basic", "DIFFMAP VERTEXCOLOR");
		ShaderVariation* noTexturePS = graphics_->GetShader(PS, "Basic", "VERTEXCOLOR");
		ShaderVariation* diffTexturePS = graphics_->GetShader(PS, "Basic", "DIFFMAP VERTEXCOLOR");
		ShaderVariation* diffMaskTexturePS = graphics_->GetShader(PS, "Basic", "DIFFMAP ALPHAMASK VERTEXCOLOR");
		ShaderVariation* alphaTexturePS = graphics_->GetShader(PS, "Basic", "ALPHA VERTEXCOLOR");

		for(unsigned i = batchStart; i < batchEnd; ++i)
		{
			const UIBatch& batch = batches[i];
			if(batch.vertexStart_ == batch.vertexEnd_)
				continue;

			ShaderVariation* ps;
			ShaderVariation* vs;
			if(!batch.texture_)
			{
				ps = noTexturePS;
				vs = noTextureVS;
			}
			else
			{
				vs = diffTextureVS;

				if(batch.texture_->GetFormat() == alphaFormat)
				{
					ps = alphaTexturePS;
				}
				else if(batch.blendMode_ != BLEND_ALPHA && batch.blendMode_ != BLEND_ADDALPHS &&
						batch.blendMode_ != BLEND_PREMULALPHA)
				{
					ps = diffMaskTexturePS;
				}
				else
				{
					ps = diffTexturePS;
				}
			}
			graphics_->SetShaders(vs, ps);
			if(graphics_->NeedParameterUpdate(SP_OBJECT, this))
			{
				graphics_->SetShaderParameter(VSP_MODEL, Matrix3x4::IDENTITY);
			}
			if(graphics_->NeedParameterUpdate(SP_CAMERA, this))
			{
				graphics_->SetShaderParameter(VSP_VIEWPROJ, projection);
			}
			if(graphics_->NeedParameterUpdate(SP_MATERIAL, this))
			{
				graphics_->SetShaderParameter(PSP_MATSPECCOLOR, Color(1.0f, 1.0f, 1.0f, 1.0f));
			}

			float elapsedTime = GetSubsystem<Time>()->GetElapsedTime();
			graphics_->SetShaderParameter(VSP_ELAPSEDTIME, elapsedTime);
			graphics_->SetShaderParameter(PSP_ELAPSEDTIME, elapsedTime);

			IntRect scissor = batch.scissor_;
			scissor.left_ = (int)(scissor.left_ * uiScale_);
			scissor.top_ = (int)(scissor.top_ * uiScale_);
			scissor.right_ = (int)(scissor.right_ * uiScale_);
			scissor.bottom_ = (int)(scissor.bottom_ * uiScale_);

#ifdef URHO3D_OEPNGL
			if(surface)
			{
				int top = scissor.top_;
				int bottom = scissor.bottom_;
				scissor.top_ = viewSize.y_ - bottom;
				scissor.bottom_ = viewSize.y_ - top;
			}
#endif

			graphics_->SetBlendMode(batch.blendMode_);
			graphics_->SetScissorTest(true, scissor);
			graphics_->SetTexture(0, batch.texture_);
			graphics_->Draw(TRIANGLE_LIST, batch.vertexStart_ / UI_VERTEX_SIZE,
					(batch.vertexEnd_ - batch.vertexStart_) / UI_VERTEX_SIZE);
		}
	}

	void RegisterUILibrary(Context *context)
	{
		UIElement::RegisterObject(context);
		Sprite::RegisterObject(context);
	}


}

