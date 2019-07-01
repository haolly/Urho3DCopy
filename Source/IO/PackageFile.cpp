//
// Created by LIUHAO on 2017/12/16.
//

#include "PackageFile.h"
#include "File.h"
#include "Log.h"

namespace Urho3D
{

	PackageFile::PackageFile(Context *context) :
			Object(context),
			totalSize_(0),
			totalDataSize_(0),
			checksum_(0),
			compressed_(false)
	{
	}

	PackageFile::PackageFile(Context *context, const String &fileName, unsigned int startOffset) :
		Object(context),
		totalSize_(0),
		totalDataSize_(0),
		checksum_(0),
		compressed_(false)
	{
		Open(fileName, startOffset);
	}

	PackageFile::~PackageFile()
	{
	}

	bool PackageFile::Open(const String &fileName, unsigned int startOffset)
	{
		SharedPtr<File> file(new File(context_, fileName));
		if(!file->IsOpen())
			return false;

		file->Seek(startOffset);
		String id = file->ReadFileID();
		if(id != "UPAK" && id != "ULZ4")
		{
			if(!startOffset)
			{
				unsigned fileSize = file->GetSize();
				file->Seek((unsigned)(fileSize - sizeof(unsigned)));
				unsigned newStartOffset = fileSize - file->ReadUInt();
				if(newStartOffset < fileSize)
				{
					startOffset = newStartOffset;
					file->Seek(startOffset);
					id = file->ReadFileID();
				}
			}

			if(id != "UPAK" && id != "ULZ4")
			{
				URHO3D_LOGERROR(fileName + " is not valid package file");
				return false;
			}
		}

		fileName_ = fileName;
		nameHash_ = fileName;
		totalSize_ = file->GetSize();
		compressed_ = id == "ULZ4";

		unsigned numFiles = file->ReadUInt();
		checksum_ = file->ReadUInt();

		for(unsigned i=0; i< numFiles; ++i)
		{
			String entryName = file->ReadString();
			PackageEntry newEntry;
			newEntry.offset_ = file->ReadUInt() + startOffset;
			totalDataSize_ += (newEntry.size_ = file->ReadUInt());
			newEntry.checkSum_ = file->ReadUInt();
			if(!compressed_ && newEntry.offset_ + newEntry.size_ > totalSize_)
			{
				URHO3D_LOGERROR("File entry " + entryName + " outside package file");
				return  false;
			}
			else
				entries_[entryName] = newEntry;
		}
		return true;
	}

	bool PackageFile::Exists(const String &fileName) const
	{
		bool found = entries_.Find(fileName) != entries_.End();
#ifdef _WIN32
		//Note, On windows, fallback to a case-insensitive search
		if(!found)
		{
			for(auto iter = entries_.Begin(); iter != entries_.End(); ++iter)
			{
				if(!iter->first_.Compare(fileName, false))
				{
					found = true;
					break;
				}
			}
		}
#endif
		return found;
	}

	const PackageEntry *PackageFile::GetEntry(const String &fileName) const
	{
		auto iter = entries_.Find(fileName);
		if(iter != entries_.End())
			return &(iter->second_);
#ifdef _WIN32
		for(auto iter = entries_.Begin(); iter != entries_.End(); ++iter)
		{
			if(!iter->first_.Compare(fileName, false))
			{
				return &(iter->second_);
			}
		}
#endif
		return nullptr;
	}
}