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
		//todo
	};

}



#endif //URHO3DCOPY_PACKAGEFILE_H
