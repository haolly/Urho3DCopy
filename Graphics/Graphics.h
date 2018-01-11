//
// Created by liuhao1 on 2018/1/4.
//

#ifndef URHO3DCOPY_GRAPHICS_H
#define URHO3DCOPY_GRAPHICS_H

#include "../Core/Object.h"
#include "../Resource/Image.h"
#include "GraphicsDefs.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

namespace Urho3D
{
	// CPU side scratch buffer for vertex data updates
	struct ScratchBuffer
	{
		ScratchBuffer() :
			size_(0),
			reserved_(false)
		{
		}

		SharedArrayPtr<unsigned char> data_;
		unsigned size_;
		bool reserved_;
	};

	class Graphics : public Object
	{
		URHO3D_OBJECT(Graphics, Object);
	public:
		Graphics(Context* context);
		virtual ~Graphics() override ;

		void SetExternalWindow(void* window);
		void SetWindowTitle(const String& windowTitle);
		void SetWindowIcon(Image* windowIcon);
		void SetWindowPosition(const IntVector2& position);
		void SetWindowPosition(int x, int y);

		bool SetMode(int width, int height, bool fullScreen, bool borderless, bool resizable, bool highDPI,
					bool vsync, bool tripleBuffer, int multiSample, int monitor, int refreshRate);

		bool SetMode(int width, int height);
		void SetSRGB(bool enable);
		//Note, Dither, 抖动
		// Set whether rendering output is dithered. Default true on OpenGL. No effect on Direct3D.
		void SetDither(bool enable);

		//Set whether to flush the GPU command buffer to prevent multiple frames being queued and uneven frame timesteps.
		void SetFlushGPU(bool enable);

		void SetForceGL2(bool enable);
		void SetOrientations(const String& orientations);
		bool ToggleFullscreen();

		void Close();
		bool TakeScreenShot(Image& destImage);

		bool BeginFrame();
		void EndFrame();
		void Clear(unsigned flags, const Color& color = Color(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0);

		void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount);
		void Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount);
		void Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned baseVertexIndex, unsigned minVertex,
		          unsigned vertexCount);

		void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount,
		                   unsigned instanceCount);
		void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned baseVertexIndex,
		                   unsigned minVertex, unsigned vertexCount, unsigned instanceCount);

		void SetVertexBuffer(VertexBuffer* buffer);
		bool SetVertexBuffers(const PODVector<VertexBuffer*>& buffers, unsigned instanceOffset = 0);
		bool SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>>& buffers, unsigned instanceOffset = 0);

		void SetIndexBuffer(IndexBuffer* buffer);

		void SetTexture(unsigned index, Texture* texture);
		//todo

		Texture* GetTexture(unsigned index) const;


		bool GetSRGBSupport() const { return sRGBSupport_; }

		TextureFilterMode GetDefaultTextureFilterMode() const
		{
			return defaultTextureFilterMode_;
		}

		unsigned GetDefaultTextureAnisotropy() const
		{
			return defaultTextureAnisotropy_;
		}

		void OnWindowResized();
		void OnWindowMoved();
		void Restore();
		void Maxmize();
		void Minimize();
		void Raise() const;

		void AddGPUObject(GPUObject* object);
		void RemoveGPUObject(GPUObject* object);

		//todo

	private:
		bool sRGBSupport_;

		TextureFilterMode defaultTextureFilterMode_;
		unsigned defaultTextureAnisotropy_;

		Texture* textures_[MAX_TEXTURE_UNITS];
	};

}



#endif //URHO3DCOPY_GRAPHICS_H
