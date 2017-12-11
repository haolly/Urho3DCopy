//
// Created by LIUHAO on 2017/12/8.
//

#ifndef URHO3DCOPY_FILE_H
#define URHO3DCOPY_FILE_H

#include "../Core/Object.h"
#include "AbstractFile.h"
#include "../Container/ArrayPtr.h"

namespace Urho3D
{
	enum FileMode
	{
		FILE_READ = 0,
		FILE_WRITE,
		FILE_READWRITE
	};

	class PackageFile;

	class File : public Object, public AbstractFile
	{
		URHO3D_OBJECT(File, Object);
	public:
		File(Context* context);
		File(Context* context, const String& fileName, FileMode mode = FILE_READ);
		File(Context* context, PackageFile* package, const String& fileName);
		virtual ~File() override;

		virtual unsigned Read(void* dest, unsigned size) override;

		unsigned int Seek(unsigned position) override;

		unsigned int Write(const void *data, unsigned size) override;

		virtual const String& GetName() const override
		{
			return fileName_;
		}

		virtual unsigned GetChecksum() override;
		bool Open(const String& fileName, FileMode mode = FILE_READ);
		bool Open(PackageFile* package, const String& fileName);
		void Close();
		void Flush();
		void SetName(const String& name);

		FileMode GetMode() const
		{
			return mode_;
		}

		bool IsOpen() const;
		void* GetHandle() const { return handle_; }
		bool IsPackaged() const { return offset_ != 0; }

	private:
		bool OpenInternal(const String& fileName, FileMode mode, bool fromPackage = false);
		bool ReadInternal(void* dest, unsigned size);
		void SeekInternal(unsigned newPosition);

		String fileName_;
		FileMode mode_;
		void* handle_;
		SharedArrayPtr<unsigned char> readBuffer_;
		SharedArrayPtr<unsigned char> inputBuffer_;

		unsigned readBufferOffset_;
		unsigned readBufferSize_;
		//Start Position within a package file, 0 for regular files
		unsigned offset_;
		unsigned checksum_;
		bool compressed_;
		bool readSyncNeeded_;
		bool writeSyncNeeded_;
	};

}



#endif //URHO3DCOPY_FILE_H
