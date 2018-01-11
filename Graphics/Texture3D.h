//
// Created by liuhao1 on 2018/1/10.
//

#ifndef URHO3DCOPY_TEXTURE3D_H
#define URHO3DCOPY_TEXTURE3D_H

#include "Texture.h"
#include "../Resource/Image.h"

namespace Urho3D
{
	class Texture3D : public Texture
	{
		URHO3D_OBJECT(Texture3D, Texture);

	public:
		Texture3D(Context* context);
		virtual ~Texture3D() override ;

		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;
		virtual bool EndLoad() override ;
		virtual void OnDeviceLost() override ;
		virtual void OnDeviceReset() override ;
		virtual void Release() override ;

		bool SetSize(int width, int height, int depth, unsigned format, TextureUsage usage = TEXTURE_STATIC);
		bool SetData(unsigned level, int x, int y, int z, int width, int height, int depth, const void* data);
		bool SetData(Image* image, bool useAlpha = false);

		bool GetData(unsigned level, void* dest) const;

	protected:
		virtual bool Create() override ;

	private:
		SharedPtr<Image> loadImage_;
		SharedPtr<XMLFile> loadParameters_;
	};

}

#endif //URHO3DCOPY_TEXTURE3D_H
