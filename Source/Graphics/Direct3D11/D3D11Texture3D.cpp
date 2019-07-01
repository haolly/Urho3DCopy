//
// Created by liuhao1 on 2018/1/10.
//

#include "../Texture3D.h"
#include "D3D11GraphicsImpl.h"
#include "../Graphics.h"

namespace Urho3D
{
	void Texture3D::OnDeviceLost()
	{
		// No-op on Direct3D11
	}

	void Texture3D::OnDeviceReset()
	{
		// No-op on Direct3D11
	}

	void Texture3D::Release()
	{
		if(graphics_ && object_.ptr_)
		{
			for(unsigned i=0; i< MAX_TEXTURE_UNITS; ++i)
			{
				if(graphics_->GetTexture(i) == this)
					graphics_->SetTexture(i, nullptr);
			}
		}

		URHO3D_SAFE_RELEASE(object_.ptr_);
		URHO3D_SAFE_RELEASE(shaderResourceView_);
		URHO3D_SAFE_RELEASE(sampler_);
	}

	bool Texture3D::Create()
	{
		Release();

		if(!graphics_ || !width_ || !height_ || !depth_)
			return false;

		levels_ = CheckMaxLevels(width_, height_, depth_, requestedLevels_);

		D3D11_TEXTURE3D_DESC textureDesc;
		memset(&textureDesc, 0, sizeof(textureDesc));
		textureDesc.Width = (UINT)width_;
		textureDesc.Height = (UINT)height_;
		textureDesc.Depth = (UINT)depth_;
		textureDesc.MipLevels = levels_;
		textureDesc.Format = (DXGI_FORMAT)(sRGB_ ? GetSRGBFormat(format_) : format_);
		textureDesc.Usage = usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

		//todo
	}

	bool Texture3D::SetData(unsigned level, int x, int y, int z, int width, int height, int depth, const void *data)
	{
		//todo
	}

	bool Texture3D::SetData(Image *image, bool useAlpha)
	{
		//todo
	}

	bool Texture3D::GetData(unsigned level, void *dest) const
	{
		//todo
	}

}
