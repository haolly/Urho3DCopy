//
// Created by LIUHAO on 2017/12/14.
//

#include "Image.h"
#include "../Core/Context.h"
#include "../IO/Log.h"
#include "../Math/MathDefs.h"
//todo make this work
//#include <STB/stb_image_write.h>
#include "../ThirdParty/STB/stb_image_write.h"

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((unsigned)(ch0) | ((unsigned)(ch1) << 8) | ((unsigned)(ch2) << 16) | ((unsigned)(ch3) << 24))
#endif

#define FOURCC_DXT1 (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2 (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3 (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4 (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5 (MAKEFOURCC('D','X','T','5'))
#define FOURCC_DX10 (MAKEFOURCC('D','X','1','0'))

static const unsigned DDSCAPS_COMPLEX = 0x00000008U;
static const unsigned DDSCAPS_TEXTURE = 0x00001000U;
static const unsigned DDSCAPS_MIPMAP = 0x00400000U;
static const unsigned DDSCAPS2_VOLUME = 0x00200000U;
static const unsigned DDSCAPS2_CUBEMAP = 0x00000200U;

static const unsigned DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400U;
static const unsigned DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800U;
static const unsigned DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000U;
static const unsigned DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000U;
static const unsigned DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000U;
static const unsigned DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000U;
static const unsigned DDSCAPS2_CUBEMAP_ALL_FACES = 0x0000FC00U;

// DX10 flags
static const unsigned DDS_DIMENSION_TEXTURE1D = 2;
static const unsigned DDS_DIMENSION_TEXTURE2D = 3;
static const unsigned DDS_DIMENSION_TEXTURE3D = 4;

static const unsigned DDS_RESOURCE_MISC_TEXTURECUBE = 0x4;

static const unsigned DDS_DXGI_FORMAT_R8G8B8A8_UNORM = 28;
static const unsigned DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 26;
static const unsigned DDS_DXGI_FORMAT_BC1_UNORM = 71;
static const unsigned DDS_DXGI_FORMAT_BC1_UNORM_SRGB = 72;
static const unsigned DDS_DXGI_FORMAT_BC2_UNORM = 74;
static const unsigned DDS_DXGI_FORMAT_BC2_UNORM_SRGB = 75;
static const unsigned DDS_DXGI_FORMAT_BC3_UNORM = 77;
static const unsigned DDS_DXGI_FORMAT_BC3_UNORM_SRGB = 78;


namespace Urho3D
{

	struct DDColorKey
	{
		unsigned dwColorSpaceLowValue_;
		unsigned dwColorSpaceHighValue_;
	};

/// DirectDraw pixel format definition.
	struct DDPixelFormat
	{
		unsigned dwSize_;
		unsigned dwFlags_;
		unsigned dwFourCC_;
		union
		{
			unsigned dwRGBBitCount_;
			unsigned dwYUVBitCount_;
			unsigned dwZBufferBitDepth_;
			unsigned dwAlphaBitDepth_;
			unsigned dwLuminanceBitCount_;
			unsigned dwBumpBitCount_;
			unsigned dwPrivateFormatBitCount_;
		};
		union
		{
			unsigned dwRBitMask_;
			unsigned dwYBitMask_;
			unsigned dwStencilBitDepth_;
			unsigned dwLuminanceBitMask_;
			unsigned dwBumpDuBitMask_;
			unsigned dwOperations_;
		};
		union
		{
			unsigned dwGBitMask_;
			unsigned dwUBitMask_;
			unsigned dwZBitMask_;
			unsigned dwBumpDvBitMask_;
			struct
			{
				unsigned short wFlipMSTypes_;
				unsigned short wBltMSTypes_;
			} multiSampleCaps_;
		};
		union
		{
			unsigned dwBBitMask_;
			unsigned dwVBitMask_;
			unsigned dwStencilBitMask_;
			unsigned dwBumpLuminanceBitMask_;
		};
		union
		{
			unsigned dwRGBAlphaBitMask_;
			unsigned dwYUVAlphaBitMask_;
			unsigned dwLuminanceAlphaBitMask_;
			unsigned dwRGBZBitMask_;
			unsigned dwYUVZBitMask_;
		};
	};

/// DirectDraw surface capabilities.
	struct DDSCaps2
	{
		unsigned dwCaps_;
		unsigned dwCaps2_;
		unsigned dwCaps3_;
		union
		{
			unsigned dwCaps4_;
			unsigned dwVolumeDepth_;
		};
	};

	struct DDSHeader10
	{
		unsigned dxgiFormat;
		unsigned resourceDimension;
		unsigned miscFlag;
		unsigned arraySize;
		unsigned reserved;
	};

	struct DDSurfaceDesc2
	{
		unsigned dwSize_;
		unsigned dwFlags_;
		unsigned dwHeight_;
		unsigned dwWidth_;
		union
		{
			unsigned lPitch_;
			unsigned dwLinearSize_;
		};
		union
		{
			unsigned dwBackBufferCount_;
			unsigned dwDepth_;
		};
		union
		{
			unsigned dwMipMapCount_;
			unsigned dwRefreshRate_;
			unsigned dwSrcVBHandle_;
		};
		unsigned dwAlphaBitDepth_;
		unsigned dwReserved_;
		unsigned lpSurface_; // Do not define as a void pointer, as it is 8 bytes in a 64bit build
		union
		{
			DDColorKey ddckCKDestOverlay_;
			unsigned dwEmptyFaceColor_;
		};
		DDColorKey ddckCKDestBlt_;
		DDColorKey ddckCKSrcOverlay_;
		DDColorKey ddckCKSrcBlt_;
		union
		{
			DDPixelFormat ddpfPixelFormat_;
			unsigned dwFVF_;
		};
		DDSCaps2 ddsCaps_;
		unsigned dwTextureStage_;
	};


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

	bool Image::BeginLoad(Deserializer& source)
	{
		// Check for DDS, KTX or PVR compressed format
		String fileID = source.ReadFileID();

		if (fileID == "DDS ")
		{
			// DDS compressed format
			DDSurfaceDesc2 ddsd;
			source.Read(&ddsd, sizeof(ddsd));

			// DDS DX10+
			const bool hasDXGI = ddsd.ddpfPixelFormat_.dwFourCC_ == FOURCC_DX10;
			DDSHeader10 dxgiHeader;
			if (hasDXGI)
				source.Read(&dxgiHeader, sizeof(dxgiHeader));

			unsigned fourCC = ddsd.ddpfPixelFormat_.dwFourCC_;

			// If the DXGI header is available then remap formats and check sRGB
			if (hasDXGI)
			{
				switch (dxgiHeader.dxgiFormat)
				{
					case DDS_DXGI_FORMAT_BC1_UNORM:
					case DDS_DXGI_FORMAT_BC1_UNORM_SRGB:
						fourCC = FOURCC_DXT1;
						break;
					case DDS_DXGI_FORMAT_BC2_UNORM:
					case DDS_DXGI_FORMAT_BC2_UNORM_SRGB:
						fourCC = FOURCC_DXT3;
						break;
					case DDS_DXGI_FORMAT_BC3_UNORM:
					case DDS_DXGI_FORMAT_BC3_UNORM_SRGB:
						fourCC = FOURCC_DXT5;
						break;
					case DDS_DXGI_FORMAT_R8G8B8A8_UNORM:
					case DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
						fourCC = 0;
						break;
					default:
						URHO3D_LOGERROR("Unrecognized DDS DXGI image format");
						return false;
				}

				// Check the internal sRGB formats
				if (dxgiHeader.dxgiFormat == DDS_DXGI_FORMAT_BC1_UNORM_SRGB ||
				    dxgiHeader.dxgiFormat == DDS_DXGI_FORMAT_BC2_UNORM_SRGB ||
				    dxgiHeader.dxgiFormat == DDS_DXGI_FORMAT_BC3_UNORM_SRGB ||
				    dxgiHeader.dxgiFormat == DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
				{
					sRGB_ = true;
				}
			}
			switch (fourCC)
			{
				case FOURCC_DXT1:
					compressedFormat_ = CF_DXT1;
					components_ = 3;
					break;

				case FOURCC_DXT3:
					compressedFormat_ = CF_DXT3;
					components_ = 4;
					break;

				case FOURCC_DXT5:
					compressedFormat_ = CF_DXT5;
					components_ = 4;
					break;

				case 0:
					if (ddsd.ddpfPixelFormat_.dwRGBBitCount_ != 32 && ddsd.ddpfPixelFormat_.dwRGBBitCount_ != 24 &&
					    ddsd.ddpfPixelFormat_.dwRGBBitCount_ != 16)
					{
						URHO3D_LOGERROR("Unsupported DDS pixel byte size");
						return false;
					}
					compressedFormat_ = CF_RGBA;
					components_ = 4;
					break;

				default:
					URHO3D_LOGERROR("Unrecognized DDS image format");
					return false;
			}

			// Is it a cube map or texture array? If so determine the size of the image chain.
			cubemap_ = (ddsd.ddsCaps_.dwCaps2_ & DDSCAPS2_CUBEMAP_ALL_FACES) != 0 || (hasDXGI && (dxgiHeader.miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE) != 0);
			unsigned imageChainCount = 1;
			if (cubemap_)
				imageChainCount = 6;
			else if (hasDXGI && dxgiHeader.arraySize > 1)
			{
				imageChainCount = dxgiHeader.arraySize;
				array_ = true;
			}

			// Calculate the size of the data
			unsigned dataSize = 0;
			if (compressedFormat_ != CF_RGBA)
			{
				const unsigned blockSize = compressedFormat_ == CF_DXT1 ? 8 : 16; //DXT1/BC1 is 8 bytes, DXT3/BC2 and DXT5/BC3 are 16 bytes
				// Add 3 to ensure valid block: ie 2x2 fits uses a whole 4x4 block
				unsigned blocksWide = (ddsd.dwWidth_ + 3) / 4;
				unsigned blocksHeight = (ddsd.dwHeight_ + 3) / 4;
				dataSize = blocksWide * blocksHeight * blockSize;

				// Calculate mip data size
				unsigned x = ddsd.dwWidth_ / 2;
				unsigned y = ddsd.dwHeight_ / 2;
				unsigned z = ddsd.dwDepth_ / 2;
				for (unsigned level = ddsd.dwMipMapCount_; level > 1; x /= 2, y /= 2, z /= 2, --level)
				{
					blocksWide = (Max(x, 1U) + 3) / 4;
					blocksHeight = (Max(y, 1U) + 3) / 4;
					dataSize += blockSize * blocksWide * blocksHeight * Max(z, 1U);
				}
			}
			else
			{
				dataSize = (ddsd.ddpfPixelFormat_.dwRGBBitCount_ / 8) * ddsd.dwWidth_ * ddsd.dwHeight_ * Max(ddsd.dwDepth_, 1U);
				// Calculate mip data size
				unsigned x = ddsd.dwWidth_ / 2;
				unsigned y = ddsd.dwHeight_ / 2;
				unsigned z = ddsd.dwDepth_ / 2;
				for (unsigned level = ddsd.dwMipMapCount_; level > 1; x /= 2, y /= 2, z /= 2, --level)
					dataSize += (ddsd.ddpfPixelFormat_.dwRGBBitCount_ / 8) * Max(x, 1U) * Max(y, 1U) * Max(z, 1U);
			}

			// Do not use a shared ptr here, in case nothing is refcounting the image outside this function.
			// A raw pointer is fine as the image chain (if needed) uses shared ptr's properly
			Image* currentImage = this;

			for (unsigned faceIndex = 0; faceIndex < imageChainCount; ++faceIndex)
			{
				currentImage->data_ = new unsigned char[dataSize];
				currentImage->cubemap_ = cubemap_;
				currentImage->array_ = array_;
				currentImage->components_ = components_;
				currentImage->compressedFormat_ = compressedFormat_;
				currentImage->width_ = ddsd.dwWidth_;
				currentImage->height_ = ddsd.dwHeight_;
				currentImage->depth_ = ddsd.dwDepth_;

				currentImage->numCompressedLevels_ = ddsd.dwMipMapCount_;
				if (!currentImage->numCompressedLevels_)
					currentImage->numCompressedLevels_ = 1;

				// Memory use needs to be exact per image as it's used for verifying the data size in GetCompressedLevel()
				// even though it would be more proper for the first image to report the size of all siblings combined
				currentImage->SetMemoryUse(dataSize);

				source.Read(currentImage->data_.Get(), dataSize);

				if (faceIndex < imageChainCount - 1)
				{
					// Build the image chain
					SharedPtr<Image> nextImage(new Image(context_));
					currentImage->nextSibling_ = nextImage;
					currentImage = nextImage;
				}
			}

			// If uncompressed DDS, convert the data to 8bit RGBA as the texture classes can not currently use eg. RGB565 format
			if (compressedFormat_ == CF_RGBA)
			{
				URHO3D_PROFILE(ConvertDDSToRGBA);

				currentImage = this;

				while (currentImage)
				{
					unsigned sourcePixelByteSize = ddsd.ddpfPixelFormat_.dwRGBBitCount_ >> 3;
					unsigned numPixels = dataSize / sourcePixelByteSize;

#define ADJUSTSHIFT(mask, l, r) \
                if (mask && mask >= 0x100) \
                { \
                    while ((mask >> r) >= 0x100) \
                    ++r; \
                } \
                else if (mask && mask < 0x80) \
                { \
                    while ((mask << l) < 0x80) \
                    ++l; \
                }

					unsigned rShiftL = 0, gShiftL = 0, bShiftL = 0, aShiftL = 0;
					unsigned rShiftR = 0, gShiftR = 0, bShiftR = 0, aShiftR = 0;
					unsigned rMask = ddsd.ddpfPixelFormat_.dwRBitMask_;
					unsigned gMask = ddsd.ddpfPixelFormat_.dwGBitMask_;
					unsigned bMask = ddsd.ddpfPixelFormat_.dwBBitMask_;
					unsigned aMask = ddsd.ddpfPixelFormat_.dwRGBAlphaBitMask_;
					ADJUSTSHIFT(rMask, rShiftL, rShiftR)
					ADJUSTSHIFT(gMask, gShiftL, gShiftR)
					ADJUSTSHIFT(bMask, bShiftL, bShiftR)
					ADJUSTSHIFT(aMask, aShiftL, aShiftR)

					SharedArrayPtr<unsigned char> rgbaData(new unsigned char[numPixels * 4]);

					switch (sourcePixelByteSize)
					{
						case 4:
						{
							unsigned* src = (unsigned*)currentImage->data_.Get();
							unsigned char* dest = rgbaData.Get();

							while (numPixels--)
							{
								unsigned pixels = *src++;
								*dest++ = ((pixels & rMask) << rShiftL) >> rShiftR;
								*dest++ = ((pixels & gMask) << gShiftL) >> gShiftR;
								*dest++ = ((pixels & bMask) << bShiftL) >> bShiftR;
								*dest++ = ((pixels & aMask) << aShiftL) >> aShiftR;
							}
						}
							break;

						case 3:
						{
							unsigned char* src = currentImage->data_.Get();
							unsigned char* dest = rgbaData.Get();

							while (numPixels--)
							{
								unsigned pixels = src[0] | (src[1] << 8) | (src[2] << 16);
								src += 3;
								*dest++ = ((pixels & rMask) << rShiftL) >> rShiftR;
								*dest++ = ((pixels & gMask) << gShiftL) >> gShiftR;
								*dest++ = ((pixels & bMask) << bShiftL) >> bShiftR;
								*dest++ = ((pixels & aMask) << aShiftL) >> aShiftR;
							}
						}
							break;

						default:
						{
							unsigned short* src = (unsigned short*)currentImage->data_.Get();
							unsigned char* dest = rgbaData.Get();

							while (numPixels--)
							{
								unsigned short pixels = *src++;
								*dest++ = ((pixels & rMask) << rShiftL) >> rShiftR;
								*dest++ = ((pixels & gMask) << gShiftL) >> gShiftR;
								*dest++ = ((pixels & bMask) << bShiftL) >> bShiftR;
								*dest++ = ((pixels & aMask) << aShiftL) >> aShiftR;
							}
						}
							break;
					}

					// Replace with converted data
					currentImage->data_ = rgbaData;
					currentImage->SetMemoryUse(numPixels * 4);
					currentImage = currentImage->GetNextSibling();
				}
			}
		}
		else if (fileID == "\253KTX")
		{
			source.Seek(12);

			unsigned endianness = source.ReadUInt();
			unsigned type = source.ReadUInt();
			/* unsigned typeSize = */ source.ReadUInt();
			unsigned format = source.ReadUInt();
			unsigned internalFormat = source.ReadUInt();
			/* unsigned baseInternalFormat = */ source.ReadUInt();
			unsigned width = source.ReadUInt();
			unsigned height = source.ReadUInt();
			unsigned depth = source.ReadUInt();
			/* unsigned arrayElements = */ source.ReadUInt();
			unsigned faces = source.ReadUInt();
			unsigned mipmaps = source.ReadUInt();
			unsigned keyValueBytes = source.ReadUInt();

			if (endianness != 0x04030201)
			{
				URHO3D_LOGERROR("Big-endian KTX files not supported");
				return false;
			}

			if (type != 0 || format != 0)
			{
				URHO3D_LOGERROR("Uncompressed KTX files not supported");
				return false;
			}

			if (faces > 1 || depth > 1)
			{
				URHO3D_LOGERROR("3D or cube KTX files not supported");
				return false;
			}

			if (mipmaps == 0)
			{
				URHO3D_LOGERROR("KTX files without explicitly specified mipmap count not supported");
				return false;
			}

			switch (internalFormat)
			{
				case 0x83f1:
					compressedFormat_ = CF_DXT1;
					components_ = 4;
					break;

				case 0x83f2:
					compressedFormat_ = CF_DXT3;
					components_ = 4;
					break;

				case 0x83f3:
					compressedFormat_ = CF_DXT5;
					components_ = 4;
					break;

				case 0x8d64:
					compressedFormat_ = CF_ETC1;
					components_ = 3;
					break;

				case 0x8c00:
					compressedFormat_ = CF_PVRTC_RGB_4BPP;
					components_ = 3;
					break;

				case 0x8c01:
					compressedFormat_ = CF_PVRTC_RGB_2BPP;
					components_ = 3;
					break;

				case 0x8c02:
					compressedFormat_ = CF_PVRTC_RGBA_4BPP;
					components_ = 4;
					break;

				case 0x8c03:
					compressedFormat_ = CF_PVRTC_RGBA_2BPP;
					components_ = 4;
					break;

				default:
					compressedFormat_ = CF_NONE;
					break;
			}

			if (compressedFormat_ == CF_NONE)
			{
				URHO3D_LOGERROR("Unsupported texture format in KTX file");
				return false;
			}

			source.Seek(source.GetPosition() + keyValueBytes);
			unsigned dataSize = (unsigned)(source.GetSize() - source.GetPosition() - mipmaps * sizeof(unsigned));

			data_ = new unsigned char[dataSize];
			width_ = width;
			height_ = height;
			numCompressedLevels_ = mipmaps;

			unsigned dataOffset = 0;
			for (unsigned i = 0; i < mipmaps; ++i)
			{
				unsigned levelSize = source.ReadUInt();
				if (levelSize + dataOffset > dataSize)
				{
					URHO3D_LOGERROR("KTX mipmap level data size exceeds file size");
					return false;
				}

				source.Read(&data_[dataOffset], levelSize);
				dataOffset += levelSize;
				if (source.GetPosition() & 3)
					source.Seek((source.GetPosition() + 3) & 0xfffffffc);
			}

			SetMemoryUse(dataSize);
		}
		else if (fileID == "PVR\3")
		{
			/* unsigned flags = */ source.ReadUInt();
			unsigned pixelFormatLo = source.ReadUInt();
			/* unsigned pixelFormatHi = */ source.ReadUInt();
			/* unsigned colourSpace = */ source.ReadUInt();
			/* unsigned channelType = */ source.ReadUInt();
			unsigned height = source.ReadUInt();
			unsigned width = source.ReadUInt();
			unsigned depth = source.ReadUInt();
			/* unsigned numSurfaces = */ source.ReadUInt();
			unsigned numFaces = source.ReadUInt();
			unsigned mipmapCount = source.ReadUInt();
			unsigned metaDataSize = source.ReadUInt();

			if (depth > 1 || numFaces > 1)
			{
				URHO3D_LOGERROR("3D or cube PVR files not supported");
				return false;
			}

			if (mipmapCount == 0)
			{
				URHO3D_LOGERROR("PVR files without explicitly specified mipmap count not supported");
				return false;
			}

			switch (pixelFormatLo)
			{
				case 0:
					compressedFormat_ = CF_PVRTC_RGB_2BPP;
					components_ = 3;
					break;

				case 1:
					compressedFormat_ = CF_PVRTC_RGBA_2BPP;
					components_ = 4;
					break;

				case 2:
					compressedFormat_ = CF_PVRTC_RGB_4BPP;
					components_ = 3;
					break;

				case 3:
					compressedFormat_ = CF_PVRTC_RGBA_4BPP;
					components_ = 4;
					break;

				case 6:
					compressedFormat_ = CF_ETC1;
					components_ = 3;
					break;

				case 7:
					compressedFormat_ = CF_DXT1;
					components_ = 4;
					break;

				case 9:
					compressedFormat_ = CF_DXT3;
					components_ = 4;
					break;

				case 11:
					compressedFormat_ = CF_DXT5;
					components_ = 4;
					break;

				default:
					compressedFormat_ = CF_NONE;
					break;
			}

			if (compressedFormat_ == CF_NONE)
			{
				URHO3D_LOGERROR("Unsupported texture format in PVR file");
				return false;
			}

			source.Seek(source.GetPosition() + metaDataSize);
			unsigned dataSize = source.GetSize() - source.GetPosition();

			data_ = new unsigned char[dataSize];
			width_ = width;
			height_ = height;
			numCompressedLevels_ = mipmapCount;

			source.Read(data_.Get(), dataSize);
			SetMemoryUse(dataSize);
		}
#ifdef URHO3D_WEBP
			else if (fileID == "RIFF")
    {
        // WebP: https://developers.google.com/speed/webp/docs/api

        // RIFF layout is:
        //   Offset  tag
        //   0...3   "RIFF" 4-byte tag
        //   4...7   size of image data (including metadata) starting at offset 8
        //   8...11  "WEBP"   our form-type signature
        const uint8_t TAG_SIZE(4);

        source.Seek(8);
        uint8_t fourCC[TAG_SIZE];
        memset(&fourCC, 0, sizeof(uint8_t) * TAG_SIZE);

        unsigned bytesRead(source.Read(&fourCC, TAG_SIZE));
        if (bytesRead != TAG_SIZE)
        {
            // Truncated.
            URHO3D_LOGERROR("Truncated RIFF data");
            return false;
        }
        const uint8_t WEBP[TAG_SIZE] = {'W', 'E', 'B', 'P'};
        if (memcmp(fourCC, WEBP, TAG_SIZE))
        {
            // VP8_STATUS_BITSTREAM_ERROR
            URHO3D_LOGERROR("Invalid header");
            return false;
        }

        // Read the file to buffer.
        size_t dataSize(source.GetSize());
        SharedArrayPtr<uint8_t> data(new uint8_t[dataSize]);

        memset(data.Get(), 0, sizeof(uint8_t) * dataSize);
        source.Seek(0);
        source.Read(data.Get(), dataSize);

        WebPBitstreamFeatures features;

        if (WebPGetFeatures(data.Get(), dataSize, &features) != VP8_STATUS_OK)
        {
            URHO3D_LOGERROR("Error reading WebP image: " + source.GetName());
            return false;
        }

        size_t imgSize(features.width * features.height * (features.has_alpha ? 4 : 3));
        SharedArrayPtr<uint8_t> pixelData(new uint8_t[imgSize]);

        bool decodeError(false);
        if (features.has_alpha)
        {
            decodeError = WebPDecodeRGBAInto(data.Get(), dataSize, pixelData.Get(), imgSize, 4 * features.width) == nullptr;
        }
        else
        {
            decodeError = WebPDecodeRGBInto(data.Get(), dataSize, pixelData.Get(), imgSize, 3 * features.width) == nullptr;
        }
        if (decodeError)
        {
            URHO3D_LOGERROR("Error decoding WebP image:" + source.GetName());
            return false;
        }

        SetSize(features.width, features.height, features.has_alpha ? 4 : 3);
        SetData(pixelData);
    }
#endif
		else
		{
			// Not DDS, KTX or PVR, use STBImage to load other image formats as uncompressed
			source.Seek(0);
			int width, height;
			unsigned components;
			unsigned char* pixelData = GetImageData(source, width, height, components);
			if (!pixelData)
			{
				URHO3D_LOGERROR("Could not load image " + source.GetName() + ": " + String(stbi_failure_reason()));
				return false;
			}
			SetSize(width, height, components);
			SetData(pixelData);
			FreeImageData(pixelData);
		}

		return true;
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
		//todo
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

	SharedPtr<Image> Image::GetNextLevel() const
	{
		return SharedPtr<Image>();
	}

	SharedPtr<Image> Image::ConvertToRGBA() const
	{
		return SharedPtr<Image>();
	}

	CompressedLevel Image::GetCompressedLevel(unsigned index) const
	{
		return CompressedLevel();
	}

	Image *Image::GetSubimage(const IntRect &rect) const
	{
		return nullptr;
	}

	SDL_Surface *Image::GetSDLSurface(const IntRect &rect)
	{
		return nullptr;
	}

	void Image::PrecalculateLevels()
	{

	}

	bool Image::HasAlphaChannel() const
	{
		return false;
	}

	bool Image::SetSubimage(const Image *image, const IntRect &rect)
	{
		return false;
	}

	void Image::CleanupLevels()
	{

	}

	void Image::GetLevels(PODVector<Image *> &levels)
	{

	}

	void Image::GetLevels(PODVector<const Image *> &levels) const
	{

	}

}

