//
// Created by liuhao1 on 2018/1/5.
//

#ifndef URHO3DCOPY_TEXTURE_H
#define URHO3DCOPY_TEXTURE_H

#include "GPUObject.h"
#include "../Resource/Resource.h"
#include "GraphicsDefs.h"

namespace Urho3D
{
	static const int MAX_TEXTURE_QUALITY_LEVELS = 3;

	class Texture : public ResourceWithMetadata, public GPUObject
	{
	public:
		Texture(Context* context);
		virtual ~Texture() override ;

		/**
		 * Set number of requested mip levels. Needs to be called before setting size
		 * The default value (0) allocates as many mip levels as necessary to reach 1x1 size. Set value to 1
		 * to disable mipmapping.
		 * Note that rendertargets need to regenerate mips dynamically after rendering, which may cost performance.
		 * Screen buffers and shadow maps allocated by Renderer will have mipmaps disabled.
		 */
		void SetNumLevels(unsigned levels);
		void SetFilterMode(TextureFilterMode filter);
		void SetAddressMode(TextureCoordinate coord, TextureAddressMode address);
		//各向异性
		void SetAnisotropy(unsigned level);
		// Set shadow compare mode. Not used in D3D9
		void SetShadowCompare(bool enable);
		void SetBorderColor(const Color& color);
		void SetSRGB(bool enable);

		void SetBackupTexture(Texture* texture);
		void SetMipsToSkip(int quality, int toSkip);

		unsigned GetFormat() const { return format_; }

		//todo

	protected:
		void CheckTextureBudget(StringHash type);

		virtual bool Create() { return true; }

		union
		{
			/// Direct3D11 shader resource view
			void* shaderResourceView_;
			/// OpenGL target
			unsigned target_;
		};

		/// Direct3D11 sampler state object
		void* sampler_;
		/// Direct3D11 resolve texture object when multisample with autoresolve is used.
		void* resolveTexture_;

		unsigned format_;
		TextureUsage usage_;

		// Current mip levels
		unsigned levels_;

		/// Requested mip levels
		unsigned requestedLevels_;

		int width_;
		int height_;
		int depth_;
		bool shadowCompare_;

		TextureFilterMode filterMode_;
		TextureAddressMode addressMode_[MAX_COORDS];
		unsigned anisotropy_;
		unsigned mipsToSkip_[MAX_TEXTURE_QUALITY_LEVELS];
		Color borderColor_;
		/// multisampling level
		int multiSample_;
		bool sRGB_;
		bool parametersDirty_;
		bool autoResolve_;
		bool resolveDirty_;
		bool levelsDirty_;
		SharedPtr<Texture> backupTexture_;
	};

}



#endif //URHO3DCOPY_TEXTURE_H
