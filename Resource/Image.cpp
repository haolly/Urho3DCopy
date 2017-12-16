//
// Created by LIUHAO on 2017/12/14.
//

#include "Image.h"
#include "../Core/Context.h"
#include "../IO/Log.h"
//todo make this work
//#include <STB/stb_image_write.h>
#include "../ThirdParty/STB/stb_image_write.h"

namespace Urho3D
{

	Image::Image(Context *context) :
			Resource(context),
			width_(0),
			height_(0),
			depth_(0),
			components_(0),
			numCompressedLevels_(0),
			cubemap_(false),
			array_(false),
			sRGB_(false),
			compressedFormat_(CF_NONE)
	{
	}

	Image::~Image()
	{

	}

	void Image::RegisterObject(Context *context)
	{
		context->RegisterFactory<Image>();
	}

	bool Image::BeginLoad(Deserializer &source)
	{
		//todo
	}

	bool Image::Save(Urho3D::Serializer &dest) const
	{
		if(IsCompressed())
		{
			URHO3D_LOGERROR("Can not save compressed iamge " + GetName());
			return false;
		}
		if(!data_)
		{
			URHO3D_LOGERROR("Can not save zero-sized image " + GetName());
			return false;
		}

		int len;
		// Todo, lack some symbol defined
		unsigned char* png = stbi_write_png_to_mem(data_.Get(), 0, width_, height_, components_, &len);
		bool success = dest.Write(png, (unsigned)len) == (unsigned)len;
		free(png);
		return success;
	}

	bool Image::SaveFile(const Urho3D::String &fileName) const
	{
	}

	bool Image::SetSize(int width, int height, unsigned components)
	{
		return false;
	}

	bool Image::SetSize(int width, int height, int depth, unsigned components)
	{
		return false;
	}

	void Image::SetData(const unsigned char *pixelData)
	{

	}

	void Image::SetPixel(int x, int y, const Color &color)
	{

	}

	void Image::SetPixel(int x, int y, int z, const Color &color)
	{

	}

	void Image::SetPixelInt(int x, int y, unsigned uintColor)
	{

	}

	void Image::SetPixelInt(int x, int y, int z, unsigned uintColor)
	{

	}

	bool Image::LoadColorLUT(Deserializer &source)
	{
		return false;
	}

	bool Image::FlipHorizontal()
	{
		return false;
	}

	bool Image::FlipVertical()
	{
		return false;
	}

	bool Image::Resize(int width, int height)
	{
		return false;
	}

	void Image::Clear(const Color &color)
	{

	}

	void Image::ClearInt(unsigned uintColor)
	{

	}

	bool Image::SaveMBP(const String &fileName) const
	{
		return false;
	}

	bool Image::SavePNG(const String &fileName) const
	{
		return false;
	}

	bool Image::SaveTGA(const String &fileName) const
	{
		return false;
	}

	bool Image::SaveJPG(const String &fileName, int quality) const
	{
		return false;
	}

	bool Image::SaveDDS(const String &fileName) const
	{
		return false;
	}

	bool Image::SaveWEBP(const String &fileName, float compression) const
	{
		return false;
	}

	Color Image::GetPixel(int x, int y) const
	{
		return Color();
	}

	Color Image::GetPixel(int x, int y, int z) const
	{
		return Color();
	}

	unsigned Image::GetPixelInt(int x, int y) const
	{
		return 0;
	}

	unsigned Image::GetPixelInt(int x, int y, int z) const
	{
		return 0;
	}

	Color Image::GetPixelBilinear(float x, float y) const
	{
		return Color();
	}

	Color Image::GetPixelTrilinear(float x, float y, float z) const
	{
		return Color();
	}

	unsigned char *Image::GetImageData(Deserializer &source, int &width, int &height, unsigned &components)
	{
		return nullptr;
	}

	void Image::FreeImageData(unsigned char *pixelData)
	{

	}

}

