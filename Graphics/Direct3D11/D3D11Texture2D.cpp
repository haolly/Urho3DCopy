//
// Created by liuhao1 on 2018/1/18.
//

#include "../Texture2D.h"
#include "../Graphics.h"

namespace Urho3D
{
	void Texture2D::Release()
	{
		if(graphics_ && object_.ptr_)
		{
			for(unsigned i=0; i< MAX_TEXTURE_UNITS; ++i)
			{
				if(graphics_->GetTexture(i) == this)
					graphics_->SetTexture(i, nullptr);
			}
		}

		if(renderSurface_)
			renderSurface_->Release();

		URHO3D_SAFE_RELEASE(object_.ptr_);
		URHO3D_SAFE_RELEASE(resolveTexture_);
		URHO3D_SAFE_RELEASE(shaderResourceView_);
		URHO3D_SAFE_RELEASE(sampler_);
	}

}