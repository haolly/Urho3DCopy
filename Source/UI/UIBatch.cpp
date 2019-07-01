//
// Created by liuhao on 2019/2/23.
//

#include "UIBatch.hpp"
#include "UIElement.hpp"
#include "../Graphics/Texture.h"

namespace Urho3D
{
	Vector3 UIBatch::posAdjust(0.0f, 0.0f, 0.0f);

	UIBatch::UIBatch()
	{
		SetDefaultColor();
	}

	UIBatch::UIBatch(UIElement *element, BlendMode blendMode, const IntRect &scissor, Texture *texture,
	                 PODVector<float> *vertexData):
	                 element_(element),
	                 blendMode_(blendMode),
	                 scissor_(scissor),
	                 texture_(texture),
	                 invTextureSize_(texture ? Vector2(1.0f / (float) texture->GetWidth(), 1.0f / (float) texture->GetHeight()) : Vector2::ONE),
	                 vertexData_(vertexData),
	                 vertexStart_(vertexData->Size()),
	                 vertexEnd_(vertexData->Size())
	{
		SetDefaultColor();
	}

	void UIBatch::SetColor(const Color &color, bool overrideAlpha)
	{
		if(!element_)
			overrideAlpha = true;

		useGradient_ = false;
		color_ = overrideAlpha ? color.ToUint() : Color(color.r_, color.g_, color.b_, color.a_ * element_->GetDerivedOpacity()).ToUint();
	}

	void UIBatch::SetDefaultColor()
	{
		if(element_)
		{
			color_ = element_->GetDerivedColor().ToUint();
			useGradient_ = element_->HasColorGradient();
		}
		else
		{
			color_ = 0xffffffff;
			useGradient_ = false;
		}
	}

	void UIBatch::AddQuad(float x, float y, float width, float height, int texOffsetX, int texOffsetY, int texWidth,
	                      int texHeight)
	{
		unsigned topLeftColor, topRightColor, bottomLeftColor, bottomRightColor;

		if(!useGradient_)
		{
			if(!(color_ & 0xff000000))
				return;

			topLeftColor = color_;
			topRightColor = color_;
			bottomLeftColor = color_;
			bottomRightColor = color_;
		}
		else
		{
			topLeftColor = GetInterpolatedColor(x, y);
			topRightColor = GetInterpolatedColor(x + width, y);
			bottomLeftColor = GetInterpolatedColor(x, y + height);
			bottomRightColor = GetInterpolatedColor(x + width, y + height);
		}

		const IntVector2& screenPos = element_->GetScreenPosition();

		//TODO, 这里的 xy 都是相对于 UIElement 的局部坐标？
		float left = x + screenPos.x_ - posAdjust.x_;
		float right = left + width;
		float top = y + screenPos.y_ - posAdjust.y_;
		float bottom = top + height;

		//NOTE: convert UV to range(0,1)
		float leftUV = texOffsetX * invTextureSize_.x_;
		float topUV = texOffsetY * invTextureSize_.y_;
		float rightUV = (texOffsetX + (texWidth ? texWidth : width)) * invTextureSize_.x_;
		float bottomUV = (texOffsetY + (texHeight ? texHeight : height)) * invTextureSize_.y_;

		unsigned begin = vertexData_->Size();
		vertexData_->Resize(begin + 6 * UI_VERTEX_SIZE);
		float* dest = &(vertexData_->At(begin));
		vertexEnd_ = vertexData_->Size();

		dest[0] = left;
		dest[1] = top;
		dest[2] = 0.0f;
		((unsigned&)dest[3]) = topLeftColor;
		dest[4] = leftUV;
		dest[5] = topUV;

		dest[6] = right;
		dest[7] = top;
		dest[8] = 0.0f;
		((unsigned&)dest[9]) = topRightColor;
		dest[10] = rightUV;
		dest[11] = topUV;

		dest[12] = left;
		dest[13] = bottom;
		dest[14] = 0.0f;
		((unsigned&)dest[15]) = bottomLeftColor;
		dest[16] = leftUV;
		dest[17] = bottomUV;

		dest[18] = right;
		dest[19] = top;
		dest[20] = 0.0f;
		((unsigned&)dest[21]) = topRightColor;
		dest[22] = rightUV;
		dest[23] = topUV;

		dest[24] = right;
		dest[25] = bottom;
		dest[26] = 0.0f;
		((unsigned&)dest[27]) = bottomRightColor;
		dest[28] = rightUV;
		dest[29] = bottomUV;

		dest[30] = left;
		dest[31] = bottom;
		dest[32] = 0.0f;
		((unsigned&)dest[33]) = topLeftColor;
		dest[34] = leftUV;
		dest[35] = bottomUV;
	}

	bool UIBatch::Merge(const UIBatch &batch)
	{
		if (batch.blendMode_ != blendMode_ ||
			batch.scissor_ != scissor_ ||
			batch.texture_ != texture_ ||
			batch.vertexData_ != vertexData_ ||
			batch.vertexStart_ != vertexStart_)
			return false;

		vertexEnd_ = batch.vertexEnd_;
		return true;
	}

	unsigned UIBatch::GetInterpolatedColor(float x, float y)
	{
		const IntVector2& size = element_->GetSize();

		if(size.x_ && size.y_)
		{
			float cLerpX = Clamp(x / (float)size.x_, 0.0f, 1.0f);
			float cLerpY = Clamp(y / (float)size.y_, 0.0f, 1.0f);

			Color topColor = element_->GetColor(C_TOPLEFT).Lerp(element_->GetColor(C_TOPRIGHT), cLerpX);
			Color bottomColor = element_->GetColor(C_BOTTOMLEFT).Lerp(element_->GetColor(C_BOTTOMRIGHT), cLerpX);
			Color color = topColor.Lerp(bottomColor, cLerpY);
			color.a_ *= element_->GetDerivedOpacity();
			return color.ToUint();
		}
		else
		{
			Color color = element_->GetColor(C_TOPLEFT);
			color.a_ *= element_->GetDerivedOpacity();
			return color.ToUint();
		}
	}

	void UIBatch::AddOrMerge(const UIBatch &batch, PODVector<UIBatch> &batches)
	{
		if(batch.vertexEnd_ == batch.vertexStart_)
			return;

		if(!batches.Empty() && batches.Back().Merge(batch))
			return;

		batches.Push(batch);
	}

	void
	UIBatch::AddQuad(const Matrix3x4 &transform, int x, int y, int width, int height, int texOffsetX, int texOffsetY,
	                 int texWidth, int texHeight)
	{

	}

}

