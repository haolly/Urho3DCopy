//
// Created by liuhao1 on 2018/1/5.
//

#ifndef URHO3DCOPY_TEXTURE_H
#define URHO3DCOPY_TEXTURE_H

#include "GPUObject.h"
#include "../Resource/Resource.h"
#include "GraphicsDefs.h"
#include "../Resource/XMLFile.h"

namespace Urho3D
{
	static const int MAX_TEXTURE_QUALITY_LEVELS = 3;

	// Base class for texture resources
	class Texture : public ResourceWithMetadata, public GPUObject
	{
		//URHO3D_OBJECT(Texture, ResourceWithMetadata);

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

		bool IsCompressed() const;

		unsigned GetLevels() const { return levels_;}

		int GetWidth() const { return width_; }

		int GetHeight() const { return height_; }

		int GetDepth() const { return depth_; }

		TextureFilterMode GetFilterMode() const { filterMode_; }

		TextureAddressMode GetAddressMode(TextureAddressMode coord) const
		{
			return addressMode_[coord];
		}

		unsigned GetAnisotropy() const { return anisotropy_; }

		bool GetShadowCompare() const { return shadowCompare_; }

		const Color& GetBorderColor() const { return borderColor_; }

		bool GetSRGB() const { return sRGB_; }

		int GetMultiSample() const { return multiSample_; }

		//Note, Resolve 溶解
		bool GetAutoResolve() const { return autoResolve_; }

		bool IsResolveDirty() const { return resolveDirty_; }

		bool GetLevelDirty() const { return levelsDirty_; }

		Texture* GetBackupTexture() const { return backupTexture_; }

		int GetMipsToSkip(int quality) const;

		int GetLevelWidth(unsigned level) const;
		int GetLevelHeight(unsigned level) const;
		int GetLevelDepth(unsigned level) const;

		TextureUsage GetUsage() const { return usage_; }

		unsigned GetDataSize(int width, int height) const;
		unsigned GetDataSize(int width, int height, int depth) const;
		unsigned GetRawDataSize(int width) const;

		unsigned GetComponents() const;

		bool GetParameterDirty() const;

		void SetParameters(XMLFile* xml);
		void SetParameters(const XMLElement& element);
		void SetParametersDirty();
		void UpdateParameters();

		void* GetShaderResourceView() const { return shaderResourceView_;}

		void* GetSampler() const { return sampler_; }

		void* GetResolveTexture() const { return resolveTexture_; }

		unsigned GetTarget() const { return target_;}
		unsigned GetSRGBFormat(unsigned format);

		void SetResolveDirty(bool enable) { resolveDirty_ = enable;}

		void SetLevelsDirty();
		void RegenerateLevels();

		static unsigned CheckMaxLevels(int width, int height, unsigned requestLevels);
		static unsigned CheckMaxLevels(int width, int height, int depth, unsigned requestedLevels);
		// Return the shader resource view format corresponding to a texture format.
		// Handles conversion of typeless depth texture formats. Only used on Direct3D11
		// ref https://msdn.microsoft.com/en-us/library/windows/desktop/ff476900(v=vs.85).aspx#Raw_Buffer_Views
		static unsigned GetSRVFormat(unsigned format);
		// depth-stencil view
		static unsigned GetDSVFormat(unsigned format);

		// Return the non-internal texture format corresponding to an OpenGL internal format
		static unsigned GetExternalFormat(unsigned format);
		// Return the data type corresponding to an OpenGL internal format
		static unsigned GetDataType(unsigned format);

	protected:
		void CheckTextureBudget(StringHash type);

		virtual bool Create() { return true; }

		union
		{
			/// Direct3D11 shader resource view
			//todo what purpose ?
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
		// todo
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
		//Todo, what is this?
		bool levelsDirty_;
		SharedPtr<Texture> backupTexture_;
	};

}



#endif //URHO3DCOPY_TEXTURE_H
