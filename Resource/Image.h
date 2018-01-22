//
// Created by LIUHAO on 2017/12/14.
//

#ifndef URHO3DCOPY_IMAGE_H
#define URHO3DCOPY_IMAGE_H

#include "Resource.h"
#include "../Math/Color.h"
#include "../Container/ArrayPtr.h"

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
		Image(Context *context);
		virtual ~Image() override ;

		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;

		bool Save(Serializer &dest) const override;

		bool SaveFile(const String &fileName) const override;

		bool SetSize(int width, int height, unsigned components);
		bool SetSize(int width, int height, int depth, unsigned components);
		void SetData(const unsigned char* pixelData);
		void SetPixel(int x, int y, const Color& color);
		void SetPixel(int x, int y, int z, const Color& color);

		void SetPixelInt(int x, int y, unsigned uintColor);
		void SetPixelInt(int x, int y, int z, unsigned uintColor);

		//Todo, color look up table ??
		bool LoadColorLUT(Deserializer& source);
		bool FlipHorizontal();
		bool FlipVertical();
		bool Resize(int width, int height);
		void Clear(const Color& color);
		void ClearInt(unsigned uintColor);

		bool SaveMBP(const String& fileName) const;
		bool SavePNG(const String& fileName) const;
		bool SaveTGA(const String& fileName) const;
		bool SaveJPG(const String& fileName, int quality) const;
		bool SaveDDS(const String& fileName) const;
		bool SaveWEBP(const String& fileName, float compression = 0.0f) const;
		bool IsCubemap() const
		{
			return cubemap_;
		}

		bool IsArray() const { return array_; }
		bool IsSRGB() const { return sRGB_; }

		Color GetPixel(int x, int y) const;
		Color GetPixel(int x, int y, int z) const;

		unsigned GetPixelInt(int x, int y) const;
		unsigned GetPixelInt(int x, int y, int z) const;
		Color GetPixelBilinear(float x, float y) const;
		Color GetPixelTrilinear(float x, float y, float z) const;

		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }
		int GetDepth() const { return depth_; }

		unsigned GetComponents() const { return components_; }
		unsigned char* GetData() const { return data_; }

		bool IsCompressed() const { return compressedFormat_ != CF_NONE; }

		CompressedFormat GetCompressedFormat() const { return compressedFormat_; }

		unsigned GetNumCompressedLevels() const { return numCompressedLevels_; }

		SharedPtr<Image> GetNextLevel() const;
		SharedPtr<Image> GetNextSibling() const { return nextSibling_; }
		SharedPtr<Image> ConvertToRGBA() const;
		CompressedLevel GetCompressedLevel(unsigned index) const;
		Image* GetSubimage(const IntRect& rect) const;
		SDL_Surface* GetSDLSurface(const IntRect& rect = IntRect::ZERO);
		// Precalculate the mip levels. Used by asynchronous texture loading
		// Note, what is mip level ??
		// Ref https://en.wikipedia.org/wiki/Mipmap, https://en.wikipedia.org/wiki/Trilinear_filtering
		void PrecalculateLevels();
		bool HasAlphaChannel() const;
		bool SetSubimage(const Image* image, const IntRect& rect);
		void CleanupLevels();
		void GetLevels(PODVector<Image*>& levels);
		void GetLevels(PODVector<const Image*>& levels) const;

	private:
		static unsigned char* GetImageData(Deserializer& source, int& width, int& height, unsigned& components);
		static void FreeImageData(unsigned char* pixelData);


		int width_;
		int height_;
		int depth_;
		unsigned components_;
		unsigned numCompressedLevels_;
		bool cubemap_;
		bool array_;
		bool sRGB_;

		CompressedFormat compressedFormat_;
		SharedArrayPtr<unsigned char> data_;
		SharedPtr<Image> nextLevel_;
		SharedPtr<Image> nextSibling_;

	};
}


#endif //URHO3DCOPY_IMAGE_H
