//
// Created by liuhao1 on 2018/1/9.
//

#ifndef URHO3DCOPY_D3D11GRAPHICSIMPL_H
#define URHO3DCOPY_D3D11GRAPHICSIMPL_H

#include <d3d11.h>
#include <dxgi.h>
#include <unknown.h>
#include "../../Container/HashMap.h"
#include "../ShaderVariation.h"
#include "D3D11ShaderProgram.h"

namespace Urho3D
{
#define URHO3D_SAFE_RELEASE(p) if(p) {((IUnknown*)p)->Release(); p = 0;}
#define URHO3D_LOGD3DERROR(msg, hr) URHO3D_LOGERRORF("%s (HRESULT %x", msg, (unsigned)hr)

using ShaderProgramMap = HashMap<Pair<ShaderVariation*, ShaderVariation>, SharedPtr<ShaderProgram>>;
using VertexDeclarationMap = HashMap<unsigned long long, SharedPtr<VertexDeclaration>>;
using ConstantBufferMap = HashMap<unsigned, SharedPtr<ConstantBuffer>>;

	class GraphicsImpl
	{
		friend class Graphics;

	public:
		GraphicsImpl();
		ID3D11Device* GetDevice() const { return device_; }

		ID3D11DeviceContext* GetDeviceContext() const { return deviceContext_; }
		IDXGISwapChain* GetSwapChain() const { return swapChain_; }
		bool CheckMultiSampleSupport(DXGI_FORMAT format, unsigned sampleCount) const;
		unsigned GetMultiSampleQuality(DXGI_FORMAT format, unsigned sampleCount) const;


	private:
		ID3D11Device* device_;
		ID3D11DeviceContext* deviceContext_;
		//todo, what is this ??
		IDXGISwapChain* swapChain_;
		ID3D11RenderTargetView* defaultRenderTargetView_;
		ID3D11Texture2D* defaultDepthTexture_;
		ID3D11DepthStencilView* defaultDepthStencilView_;
		ID3D11RenderTargetView* renderTargetViews_[MAX_RENDERTARGETS];
		ID3D11DepthStencilView* depthStencilView_;
		HashMap<unsigned, ID3D11BlendState*> blendStates_;
		HashMap<unsigned, ID3D11DepthStencilState*> depthStates_;
		HashMap<unsigned, ID3D11RasterizerState*> rasterizerStates_;

		ID3D11Texture2D* resolveTexture_;
		// todo, What does texture have to do with this ?
		ID3D11ShaderResourceView* shaderResourceView_[MAX_TEXTURE_UNITS];
		ID3D11SamplerState* samplers_[MAX_TEXTURE_UNITS];
		ID3D11Buffer* vertexBuffers_[MAX_VERTEX_STREAMS];
		ID3D11Buffer* constantBuffers_[2][MAX_SHADER_PARAMETER_GROUPS];
		unsigned vertexSize_[MAX_VERTEX_STREAMS];
		unsigned vertexOffsets_[MAX_VERTEX_STREAMS];

		bool renderTargetDirty_;
		bool texturesDirty_;
		bool vertexDeclarationDirty_;
		bool blendStateDirty_;
		bool depthStateDirty_;
		bool rasterizerStateDirty_;
		//todo, what is this ?
		bool scissorRectDirty_;
		bool stencilRefDirty_;

		unsigned blendStateHash_;
		unsigned depthStateHash_;
		unsigned rasterizerStateHash_;
		unsigned firstDirtyTexture_;
		unsigned lastDirtyTexture_;
		unsigned firstDirtyVB_;
		unsigned lastDirtyVB_;

		VertexDeclarationMap vertexDecleartions_;
		ConstantBufferMap allConstantBuffers_;
		PODVector<ConstantBuffer*> dirtyConstantBuffers_;
		ShaderProgramMap shaderPrograms_;
		ShaderProgram* shaderProgram_;
	};

}

#endif //URHO3DCOPY_D3D11GRAPHICSIMPL_H
