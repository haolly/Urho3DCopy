//
// Created by LIUHAO on 2017/12/8.
//

#include "File.h"

namespace Urho3D
{
	unsigned int File::Seek(unsigned position)
	{
		return 0;
	}

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

		//todo
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

