//
// Created by LIUHAO on 2017/12/8.
//

#include "File.h"
#include "MemoryBuffer.h"
#include "LZ4/lz4.h"
#include "FileSystem.h"
#include "../Math/MathDefs.h"
#include "Log.h"
#include "../Core/StringUtils.h"

namespace Urho3D
{
#ifdef _WIN32
	static const wchar_t* openMode[] =
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
		if(!IsOpen())
		{
			return 0;
		}

		if(mode_ == FILE_READ && position > size_)
		{
			position = size_;
		}

		if(compressed_)
		{
			if(position == 0)
			{
				position_ = 0;
				readBufferOffset_ = 0;
				readBufferSize_ = 0;
				SeekInternal(offset_);
			}
			else if(position >= position_)
			{
				unsigned char skipBuffer[SKIP_BUFFER_SIZE];
				while(position > position_)
					Read(skipBuffer, Min(position - position_, SKIP_BUFFER_SIZE));
			}
			else
			{
				//todo log error("Seeking backward in a compressed file is not supported")
			}
			return position_;
		}
		SeekInternal(position + offset_);
		position_ = position;
		readSyncNeeded_ = false;
		writeSyncNeeded_ = false;
		return position_;
	}

	unsigned int File::Write(const void *data, unsigned size)
	{
		if(!IsOpen())
		{
			return 0;
		}

		if(mode_ == FILE_READ)
		{
			//log error ("File not opened for writing");
			return 0;
		}

		if(!size)
			return 0;

		// Need to reassign the position due to internal buffering when transitioning from reading to writing
		// todo, why
		if(writeSyncNeeded_)
		{
			fseek((FILE*)handle_, position_ + offset_, SEEK_SET);
			writeSyncNeeded_ = false;
		}

		if(fwrite(data, size, 1, (FILE*)handle_) != 1)
		{
			// Rollback to the position where the write begin
			fseek((FILE*)handle_, position_ + offset_, SEEK_SET);
			//log error("Error while wirting to file: GetName()")
			return 0;
		}

		readSyncNeeded_ = true;
		position_ += size;
		if(position_ > size_)
			size_ = position_;

		return size;
	}

	bool File::Open(const String &fileName, FileMode mode)
	{
		return OpenInternal(fileName, mode);
	}

	bool File::Open(PackageFile *package, const String &fileName)
	{
		//todo
		return false;
	}

	unsigned File::GetChecksum()
	{
		if(offset_ || checksum_)
			return checksum_;
#ifdef __ANDROID__
		//todo
#else
		if(!handle_ || mode_ == FILE_WRITE)
#endif
			return 0;

		//todo profileing
		unsigned oldPos = position_;
		checksum_ = 0;

		Seek(0);
		while(!IsEof())
		{
			unsigned char block[1024];
			unsigned readBytes = Read(block, 1024);
			for(unsigned i=0; i< readBytes; ++i)
				checksum_ = SDBMHash(checksum_, block[i]);
		}
		Seek(oldPos);
		return checksum_;
	}


	void File::Close()
	{
#ifdef __ANDROID__
		//todo
#else
		readBuffer_.Reset();
		inputBuffer_.Reset();
		if(handle_)
		{
			fclose((FILE*)handle_);
			handle_ = nullptr;
			position_ = 0;
			size_ = 0;
			offset_ = 0;
			checksum_ = 0;
		}
#endif
	}

	void File::Flush()
	{
		if(handle_)
			fflush((FILE*)handle_);
	}

	void File::SetName(const String &name)
	{
		fileName_ = name;
	}

	bool File::IsOpen() const
	{
#ifdef __ANDROID__
		//todo
#else
		return handle_ != nullptr;
#endif
	}

	bool File::OpenInternal(const String& fileName, FileMode mode, bool fromPackage)
	{
		Close();

		compressed_ = false;
		readSyncNeeded_ = false;
		writeSyncNeeded_ = false;

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if (fileSystem && !fileSystem->CheckAccess(GetPath(fileName)))
		{
			URHO3D_LOGERRORF("Access denied to %s", fileName.CString());
			return false;
		}

		if(fileName.Empty())
		{
			URHO3D_LOGERROR("Could not open file with empty name");
			return false;
		}

#ifdef __ANDROID__
		//todo
#endif

#ifdef _WIN32
		handle_ = _wfopen(GetWideNativePath(fileName).CString(), openMode[mode]);
#else
		handle_ = fopen(GetNativePath(fileName).CString(), openMode[mode]);
#endif
		if(mode == FILE_READWRITE && !handle_)
		{
#ifdef _WIN32
			handle_ = _wfopen(GetWideNativePath(fileName).CString(), openMode[mode + 1]);
#else
			//todo
#endif
		}

		if(!handle_)
		{
			URHO3D_LOGERRORF("Could not open file %s ", fileName.CString());
			return false;
		}

		if(!fromPackage)
		{
			fseek((FILE*)handle_, 0, SEEK_END);
			long size = ftell((FILE*)handle_);
			fseek((FILE*)handle_, 0, SEEK_SET);
			if(size > M_MAX_UNSIGNED)
			{
				//log error()
				Close();
				size_ = 0;
				return false;
			}
			size_ = (unsigned)size;
			offset_ = 0;
		}

		fileName_ = fileName;
		mode_ = mode;
		position_ = 0;
		checksum_ = 0;
		return true;
	}

	bool File::ReadInternal(void *dest, unsigned size)
	{
#ifdef __ANDROID__
#endif
		return fread(dest, size, 1, (FILE*)handle_) == 1;
	}

	void File::SeekInternal(unsigned newPosition)
	{
#ifdef __ANDROID__
		//todo
#endif
		fseek((FILE*)handle_, newPosition, SEEK_SET);

	}

}

