//
// Created by liuhao1 on 2018/1/18.
//

#ifndef URHO3DCOPY_TEXTURE2D_H
#define URHO3DCOPY_TEXTURE2D_H

#include "Texture.h"
#include "../Resource/Image.h"
#include "RenderSurface.h"

namespace Urho3D
{
	class Texture2D : public Texture
	{
		URHO3D_OBJECT(Texture2D, Texture);

	public:
		Texture2D(Context* context);
		virtual ~Texture2D() override ;

		static void RegisterObject(Context* context);
		virtual bool BeginLoad(Deserializer& source) override ;
		virtual bool EndLoad() override;
		virtual void OnDeviceLost() override;
		virtual void OnDeviceReset() override;
		virtual void Release() override;

		/***
		 * Auto resolve true means the multisampled texture will be automatically resolved to 1-sample after being rendered to and before
		 * being sampled as a texture.
		 * Auto resolve false means the multisampled texture will be read as individual samples in the shader and is not supported on D3D9
		 * @param width
		 * @param height
		 * @param format
		 * @param usage
		 * @param multiSample
		 * @param autoResolve
		 * @return
		 */
		bool SetSize(int width, int height, unsigned format, TextureUsage usage = TEXTURE_STATIC, int multiSample = 1, bool autoResolve = true);

		// Set data either partially or fully on a mip level
		bool SetData(unsigned level, int x, int y, int width, int height, const void* data);
		bool SetData(Image* image, bool useAlpha = false);

		bool GetData(unsigned level, void* dest) const;
		// Get Image Data from zero mip level. Only RGB and RGBA textures are supported
		bool GetData(Image& image) const;
		// Get Image Data from zero mip level. Only RGB and RGBA textures are supported
		SharedPtr<Image> GetImage() const;
		RenderSurface* GetRenderSurface() const { return renderSurface_;}

	protected:
		/// Create the GPU texture
		virtual bool Create() override;

	private:
		void HandleRenderSurfaceUpdate(StringHash eventType, VariantMap& eventData);

		SharedPtr<RenderSurface> renderSurface_;
		SharedPtr<Image> loadImage_;
		SharedPtr<XMLFile> loadParameters_;
	};
}


#endif //URHO3DCOPY_TEXTURE2D_H
