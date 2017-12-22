//
// Created by LIUHAO on 2017/12/16.
//

#ifndef URHO3DCOPY_PACKAGEFILE_H
#define URHO3DCOPY_PACKAGEFILE_H

#include "../Core/Object.h"

namespace Urho3D
{
	struct PackageEntry
	{
		unsigned offset_;
		unsigned size_;
		unsigned checkSum_;
	};

	class PackageFile : public Object
	{
		URHO3D_OBJECT(PackageFile, Object);
	public:
		PackageFile(Context* context);
		PackageFile(Context* context, const String& fileName, unsigned startOffset = 0);
		virtual ~PackageFile() override ;

		bool Open(const String& fileName, unsigned startOffset = 0);
		bool Exists(const String& fileName) const;

		const PackageEntry* GetEntry(const String& fileName) const;

		HashMap<String, PackageEntry>& GetEntries() const { return entries_; };

		const String& GetName() const { return fileName_; }

		StringHash GetNameHash() const { return nameHash_; }

		unsigned GetNumFiles() const { return entries_.Size(); }

		unsigned GetTotalSize() const { return totalSize_; }

		unsigned GetTotalDataSize() const { return totalDataSize_; }

		unsigned GetChecksum() const { return checksum_; }

		bool IsCompressed() const { return compressed_; }

		const Vector<String> GetEntryNames() const { return entries_.Keys(); }

	private:
		HashMap<String, PackageEntry> entries_;
		String fileName_;
		StringHash nameHash_;
		unsigned totalSize_;
		unsigned totalDataSize_;
		unsigned checksum_;
		bool compressed_;
	};

}



#endif //URHO3DCOPY_PACKAGEFILE_H
