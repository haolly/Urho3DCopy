//
// Created by LIUHAO on 2017/12/14.
//

#ifndef URHO3DCOPY_IMAGE_H
#define URHO3DCOPY_IMAGE_H

#include "Resource.h"

struct SDL_Surface;

namespace Urho3D
{
	static const int COLOR_LUT_SIZE = 16;

	enum CompressedFormat
	{
		CF_NONE = 0,
		CF_RGBA,
		CF_DXT1,
		CF_DXT3,
		CF_DXT5,
		CF_ETC1,
		CF_PVRTC_RGB_2BPP,
		CF_PVRTC_RGBA_2BPP,
		CF_PVRTC_RGB_4BPP,
		CF_PVRTC_RGBA_4BPP,
	};

	struct CompressedLevel
	{
		CompressedLevel() :
			data_(nullptr),
			format_(CF_NONE),
			width_(0),
			height_(0),
			depth_(0),
			blockSize_(0),
			dataSize_(0),
			rowSize_(0),
			rows_(0)
		{

		}

		// Decompress to RGBA, The destination buffer required width* height * 4 bytes
		bool Decompress(unsigned char* dest);

		unsigned char* data_;
		CompressedFormat format_;
		int width_;
		int height_;
		int depth_;
		//todo
		unsigned blockSize_;

		//Total data size in bytes
		unsigned dataSize_;
		unsigned rowSize_;
		unsigned rows_;
	};

	class Image : public Resource
	{
		URHO3D_OBJECT(Image, Resource);
	public:
		Image(Context* context);
		virtual ~Image() override ;

		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;

		bool Save(Serializer &dest) const override;

		bool SaveFile(const String &fileName) const override;

		bool SetSize(int width, int height, unsigned components);
		bool SetSize(int width, int height, int depth, unsigned components);
		void SetData(const unsigned char* pixelData);
		//todo

	};
}


#endif //URHO3DCOPY_IMAGE_H
