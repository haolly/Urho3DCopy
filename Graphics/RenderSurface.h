//
// Created by liuhao1 on 2018/1/18.
//

#ifndef URHO3DCOPY_RENDERSURFACE_H
#define URHO3DCOPY_RENDERSURFACE_H

#include "../Container/RefCounted.h"
#include "GraphicsDefs.h"
#include "../Container/Ptr.h"

namespace Urho3D
{
	class Texture;

	// Color or depth-stencil surface that can be rendered into
	// todo Back buffer is also this ??
	class RenderSurface : public RefCounted
	{
		friend class Texture2D;
		friend class Texture2DArray;
		friend class TextureCube;

	public:
		RenderSurface(Texture* parentTexture);
		virtual ~RenderSurface() override ;

		void SetNumViewports(unsigned num);
		void SetViewport(unsigned index, Viewport* viewport);
		void SetUpdateMode(RenderSurfaceUpdateMode mode);
		void SetLinkedRenderTarget(RenderSurface* renderTarget);
		void SetLinkedDepthStencil(RenderSurface* depthStencil);

		void QueueUpdate();
		void Release();

		// Mark the GPU resource destroyed on graphics context destruction. Only used on OpenGL
		void OnDeviceLost();
		// Create renderbuffer that cannot be sampled as a texture. Only used on OpenGL
		bool CreateRenderBuffer(unsigned width, unsigned height, unsigned format, int multiSample);

		int GetWidth();
		int GetHeight();

		TextureUsage GetUsage() const;

		int GetMultiSample() const;
		bool GetAutoResolve() const;
		unsigned GetNumViewports() const { return viewports_.Size(); }
		//todo

		RenderSurfaceUpdateMode GetUpdateMode() const { return updateMode_; }
		RenderSurface* GetLinkedRenderTarget() const { return linkedRenderTarget_; }
		RenderSurface* GetLinkedDepthStencil() const { return linkedDepthStencil_; }
		bool IsUpdateQueued() const { return updateQueued_; }
		void ResetUpdateQueued();
		Texture* GetParentTexture() const { return parentTexture_; }
		void* GetSurface() const { return surface_; }
		void* GetRenderTargetView() const { return renderTargetView_; }
		void* GetReadOnlyView() const { return readOnlyView_; }
		unsigned GetTarget() const { return target_; }
		//todo
		bool IsResolveDirty() const { return resolveDirty_; }
		void SetResolveDirty(bool enable) { resolveDirty_ = enable; }
	private:
		Texture* parentTexture_;

		union
		{
			// Direct3D9 surface
			void* surface_;
			// Direct3D11 rendertarget or depth-stencil view
			void* renderTargetView_;
			// OpenGL renderbuffer name
			unsigned renderBuffer_;
		};

		union
		{
			// Direct3D11 read-only depth-stencil view. Present only on depth-stencil surface
			void* readOnlyView_;
			// OpenGL target
			unsigned target_;
		};

		WeakPtr<RenderSurface> linkedRenderTarget_;
		WeakPtr<RenderSurface> linkedDepthStencil_;
		RenderSurfaceUpdateMode updateMode_;
		bool updateQueued_;
		bool resolveDirty_;
	};
}



#endif //URHO3DCOPY_RENDERSURFACE_H
