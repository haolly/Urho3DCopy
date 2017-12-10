//
// Created by LIUHAO on 2017/12/8.
//

#include "File.h"
#include "MemoryBuffer.h"
#include "../ThirdParty/LZ4/lz4.h"
#include "FileSystem.h"

namespace Urho3D
{
#ifdef _WIN32
	static const char* openMode[] =
			{
					L"rb",
					L"wb",
					L"r+b",
					L"w+b"
			};
#else
	static const char* openMode[] =
			{
					"rb",
					"wb",
					"r+b",
					"w+b"
			};
#endif

	static const unsigned SKIP_BUFFER_SIZE = 1024;


	File::File(Context *context) :
		Object(context),
		mode_(FILE_READ),
		handle_(nullptr),
		readBufferOffset_(0),
		readBufferSize_(0),
		offset_(0),
		checksum_(0),
		compressed_(false),
		readSyncNeeded_(false),
		writeSyncNeeded_(false)
	{
	}

	File::File(Context *context, const String &fileName, FileMode mode) :
		Object(context),
		mode_(FILE_READ),
		handle_(nullptr),
		readBufferOffset_(0),
		readBufferSize_(0),
		offset_(0),
		checksum_(0),
		compressed_(false),
		readSyncNeeded_(false),
		writeSyncNeeded_(false)
	{
		Open(fileName, mode);
	}

	File::File(Context *context, PackageFile *package, const String &fileName) :
		Object(context),
		mode_(FILE_READ),
		handle_(nullptr),
		readBufferOffset_(0),
		readBufferSize_(0),
		offset_(0),
		checksum_(0),
		compressed_(false),
		readSyncNeeded_(false),
		writeSyncNeeded_(false)
	{
		Open(package, fileName);
	}

	File::~File()
	{
		Close();
	}

	unsigned File::Read(void *dest, unsigned size)
	{
		if (!IsOpen())
		{
			return 0;
		}

		if (mode_ == FILE_WRITE)
		{
			//todo log error("File not opened for reading")
			return 0;
		}
		if (size + position_ > size_)
			size = size_ - position_;

		if(!size)
			return 0;

#ifdef __ANDROID__
		//todo
#endif
		if(compressed_)
		{
			unsigned sizeLeft = size;
			unsigned char* destPtr = (unsigned char*)dest;
			while(sizeLeft)
			{
				if(!readBuffer_ || readBufferOffset_ >= readBufferSize_)
				{
					unsigned char blockHeaderBytes[4];
					ReadInternal(blockHeaderBytes, sizeof(blockHeaderBytes));
					MemoryBuffer blockHeader(&blockHeaderBytes[0], sizeof(blockHeaderBytes));
					unsigned unpackedSize = blockHeader.ReadUShort();
					unsigned packedSize = blockHeader.ReadUShort();

					if(!readBuffer_)
					{
						readBuffer_ = new unsigned char[unpackedSize];
						inputBuffer_ = new unsigned char[LZ4_compressBound(unpackedSize)];
					}

					ReadInternal(inputBuffer_.Get(), packedSize);
					LZ4_decompress_fast((const char*)inputBuffer_.Get(), (char*)readBuffer_.Get(), unpackedSize);

					readBufferSize_ = unpackedSize;
					readBufferOffset_ = 0;
				}

				unsigned copySize = Min((readBufferSize_ - readBufferOffset_), sizeLeft);
				memcpy(destPtr, readBuffer_.Get() + readBufferOffset_, copySize);
				destPtr += copySize;
				sizeLeft -= copySize;
				readBufferOffset_ += copySize;
				position_ += copySize;
			}
			return size;
		}

		//todo, under what circumastance this will be set ?
		if(readSyncNeeded_)
		{
			SeekInternal(position_ + offset_);
			readSyncNeeded_ = false;
		}

		if (!ReadInternal(dest, size))
		{
			//Return to the position where the read began
			SeekInternal(position_ + offset_);
			//todo log error("Error while reading from file" + GetName())
			return 0
		}

		//todo, why do this ?
		writeSyncNeeded_ = true;
		position_ += size;
		return size;
	}

	unsigned int File::Seek(unsigned position)
	{
		return 0;
	}

	unsigned int File::Write(const void *data, unsigned size)
	{
		return 0;
	}

	bool File::Open(const String &fileName, FileMode mode)
	{
		return OpenInternal(fileName, mode);
	}

	unsigned File::GetChecksum() const
	{
		return 0;
	}

	bool File::Open(PackageFile *package, const String &fileName)
	{
		//todo
		return false;
	}

	void File::Close()
	{

	}

	void File::Flush()
	{

	}

	void File::SetName(const String &name)
	{

	}

	bool File::IsOpen() const
	{
		return false;
	}

	bool File::OpenInternal(const String &fileName, FileMode mode, bool fromPackage)
	{
		Close();

		compressed_ = false;
		readSyncNeeded_ = false;
		writeSyncNeeded_ = false;

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if (fileSystem && !fileSystem->CheckAccess(GetPath(fileName)));
		{
			//todo log error ("Access denied to %s", fileName.CString()
			return false;
		}

		if(fileName.Empty())
		{
			//todo log error("Could not open file with empty name")
			return false;
		}

#ifdef __ANDROID__
		//todo
#endif

#ifdef _WIN32
		//todo
#else
		handle_ = fopen(GetNativePath(fileName).CString(), openMode[mode]);
#endif
		if(mode == FILE_READWRITE && !handle_)
		{
			//todo
		}
	}

	bool File::ReadInternal(void *dest, unsigned size)
	{
		return false;
	}

	void File::SeekInternal(unsigned newPosition)
	{

	}

	unsigned int Write(const void *data, unsigned size)
	{
		return 0;
	}

}

