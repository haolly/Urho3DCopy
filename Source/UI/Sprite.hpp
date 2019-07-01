//
// Created by liuhao on 2019-04-05.
//

#ifndef URHO3DCOPY_SPRITE_HPP
#define URHO3DCOPY_SPRITE_HPP

#include "UIElement.hpp"
#include "../Graphics/Texture.h"

namespace Urho3D
{
	class Sprite : public UIElement
	{
		URHO3D_OBJECT(Sprite, UIElement);

	public:
		explicit Sprite(Context* context);
		~Sprite() override ;

		static void RegisterObject(Context* context);

		bool IsWithinScissor(const IntRect& currentScissor) override ;

		const IntVector2& GetScreenPosition() const override ;
		void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor) override ;
		void OnPositionSet(const IntVector2& newPosition) override ;
		IntVector2 ScreenToElement(const IntVector2& screenPosition) override ;
		IntVector2 ElementToScreen(const IntVector2& position) override ;

		void SetPosition(const Vector2& position);
		void SetPosition(float x, float y);

		void SetHotSpot(const IntVector2& hotSpot);
		void SetHotSpot(int x, int y);
		void SetScale(const Vector2& scale);
		void SetScale(float x, float y);
		void SetScale(float scale);

		void SetRotation(float angle);
		void SetTexture(Texture* texture);
		void SetImageRect(const IntRect& rect);
		void SetFullImageRect();
		void SetBlendMode(BlendMode mode);

		const Vector2& GetPosition() const
		{
			return floatPosition_;
		}

		const IntVector2& GetHotSpot() const { return hotSpot_; }
		const Vector2& GetScale() const { return scale_; }

		float GetRotation() const { return rotation_; }
		Texture* GetTexture() const { return texture_; }
		const IntRect& GetImageRect() const { return imageRect_; }
		BlendMode GetBlendMode() const { return blendMode_; }

		void SetTextureAttr(const ResourceRef& value);
		ResourceRef GetTextureAttr() const;
		const Matrix3x4& GetTransform() const;


	private:
		Vector2 floatPosition_;
		IntVector2 hotSpot_;
		Vector2 scale_;
		float rotation_;
		SharedPtr<Texture> texture_;
		IntRect imageRect_;
		BlendMode blendMode_;
		mutable Matrix3x4 transform_;


	};
}



#endif //URHO3DCOPY_SPRITE_HPP
