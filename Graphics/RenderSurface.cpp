//
// Created by liuhao1 on 2018/1/18.
//

#include "RenderSurface.h"
#include "Texture.h"

namespace Urho3D
{

	RenderSurface::~RenderSurface()
	{
		Release();
	}

	void RenderSurface::SetNumViewports(unsigned num)
	{
		//todo
	}

	void RenderSurface::SetViewport(unsigned index, Viewport *viewport)
	{

	}

	void RenderSurface::SetUpdateMode(RenderSurfaceUpdateMode mode)
	{
		updateMode_ = mode;
	}

	void RenderSurface::SetLinkedRenderTarget(RenderSurface *renderTarget)
	{
		if(renderTarget != this)
			linkedRenderTarget_ = renderTarget;
	}

	void RenderSurface::SetLinkedDepthStencil(RenderSurface *depthStencil)
	{
		if(depthStencil != this)
			linkedDepthStencil_ = depthStencil;
	}

	void RenderSurface::QueueUpdate()
	{
		updateQueued_ = true;
	}

	int RenderSurface::GetWidth()
	{
		return parentTexture_->GetWidth();
	}

	int RenderSurface::GetHeight()
	{
		return parentTexture_->GetHeight();
	}

	TextureUsage RenderSurface::GetUsage() const
	{
		return parentTexture_->GetUsage();
	}

	int RenderSurface::GetMultiSample() const
	{
		return parentTexture_->GetMultiSample();
	}

	bool RenderSurface::GetAutoResolve() const
	{
		return parentTexture_->GetAutoResolve();
	}
	//todo
}