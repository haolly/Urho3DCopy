//
// Created by liuhao1 on 2018/1/9.
//

#include "D3D11GraphicsImpl.h"

namespace Urho3D
{
	GraphicsImpl::GraphicsImpl() :
		device_(nullptr),
		deviceContext_(nullptr),
		swapChain_(nullptr),
		defaultRenderTargetView_(nullptr),
		defaultDepthTexture_(nullptr),
		defaultDepthStencilView_(nullptr),
		depthStencilView_(nullptr),
		resolveTexture_(nullptr),
		shaderProgram_(nullptr)
	{
		for(unsigned i=0; i< MAX_RENDERTARGETS; ++i)
			renderTargetViews_[i] = nullptr;

		for(unsigned i=0; i< MAX_TEXTURE_UNITS; ++i)
		{
			shaderResourceView_[i] = nullptr;
			samplers_[i] = nullptr;
		}

		for(unsigned i=0; i< MAX_VERTEX_STREAMS; ++i)
		{
			vertexBuffers_[i] = nullptr;
			vertexSize_[i] = 0;
			vertexOffsets_[i] = 0;
		}

		for(unsigned i=0; i< MAX_SHADER_PARAMETER_GROUPS; ++i)
		{
			constantBuffers_[VS][i] = nullptr;
			constantBuffers_[PS][i] = nullptr;
		}
	}

	bool GraphicsImpl::CheckMultiSampleSupport(DXGI_FORMAT format, unsigned sampleCount) const
	{
		if(sampleCount < 2)
			return true;    // Not multisampled

		UINT numLevels = 0;
		if(FAILED(device_->CheckMultisampleQualityLevels(format, sampleCount, &numLevels)))
			return false;
		else
			return numLevels > 0;
	}

	unsigned GraphicsImpl::GetMultiSampleQuality(DXGI_FORMAT format, unsigned sampleCount) const
	{
		if(sampleCount < 2)
			return 0;   // Not multisampled, should use quality 0

		if(device_->GetFeatureLevel() > D3D_FEATURE_LEVEL_10_1)
			return 0xffffffff;
		UINT numLevels = 0;
		if(FAILED(device_->CheckMultisampleQualityLevels(format, sampleCount, &numLevels)))
			return 0;
		else
			return numLevels - 1;   // D3D10.0 and below: use the best quality
	}
}