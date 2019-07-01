//
// Created by liuhao1 on 2018/1/18.
//
#include "../RenderSurface.h"
#include "../Graphics.h"

namespace Urho3D
{
	RenderSurface::RenderSurface(Texture *parentTexture) :
		parentTexture_(parentTexture),
		renderTargetView_(nullptr),
		readOnlyView_(nullptr),
		updateMode_(SURFACE_UPDATEVISIBLE),
		updateQueued_(false)
	{

	}

	void RenderSurface::Release()
	{
		Graphics* graphics = parentTexture_->GetGraphics();
		if(graphics && renderTargetView_)
		{
			for(unsigned i=0; i< MAX_RENDERTARGETS; ++i)
			{
				if(graphics->GetRenderTarget(i) == this)
					graphics->ResetRenderTarget(i);
			}

			if(graphics->GetDepthStencil() == this)
				graphics->ResetDepthStencil();
		}

		URHO3D_SAFE_RELEASE(renderTargetView_);
		URHO3D_SAFE_RELEASE(readOnlyView_);
	}

	bool RenderSurface::CreateRenderBuffer(unsigned width, unsigned height, unsigned format, int multiSample)
	{
		// Note used On Direct3D
		return false;
	}

	void RenderSurface::OnDeviceLost()
	{
		// No-op on Direct3D
	}
}
