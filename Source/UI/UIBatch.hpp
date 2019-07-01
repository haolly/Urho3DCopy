//
// Created by liuhao on 2019/2/23.
//

#ifndef URHO3DCOPY_UIBATCH_HPP
#define URHO3DCOPY_UIBATCH_HPP

#include "../Math/Rect.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Math/Color.h"
#include "UI.hpp"


namespace Urho3D
{
	class Graphics;
	class Matrix3x4;
	class Texture;
	class UIElement;

	static const unsigned UI_VERTEX_SIZE = 6;

	class UIBatch
	{
	public:
		UIBatch();
		UIBatch(UIElement* element, BlendMode blendMode, const IntRect& scissor, Texture* texture, PODVector<float>* vertexData);

		void SetColor(const Color& color, bool overrideAlpha = false);
		void SetDefaultColor();
		void AddQuad(float x, float y, float width, float height, int texOffsetX, int texOffsetY, int texWidth = 0, int texHeight = 0);
		void AddQuad(const Matrix3x4& transform, int x, int y, int width, int height, int texOffsetX, int texOffsetY, int texWidth = 0, int texHeight = 0);
		void AddQuad(int x, int y, int width, int height, int texOffsetX, int texOffsetY, int texWidth, int texHeight, bool tiled);
		void AddQuad(const Matrix3x4& transform, const IntVector2& a, const IntVector2& b, const IntVector2& c, const IntVector2& d,
					const IntVector2& texA, const IntVector2& texB, const IntVector2& texC, const IntVector2& texD, const Color& colA,
					const Color& colB, const Color& colC, const Color& colD);

		bool Merge(const UIBatch& batch);

		unsigned GetInterpolatedColor(float x, float y);

		static void AddOrMerge(const UIBatch& batch, PODVector<UIBatch>& batches);

		UIElement* element_{};
		BlendMode blendMode_{BLEND_REPLACE};
		IntRect scissor_;
		Texture* texture_{};
		Vector2 invTextureSize_{Vector2::ONE};
		PODVector<float>* vertexData_{};
		unsigned vertexStart_{};
		unsigned vertexEnd_{};
		unsigned color_{};
		bool useGradient_{};

		static Vector3 posAdjust;

	};

}



#endif //URHO3DCOPY_UIBATCH_HPP
