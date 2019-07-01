//
// Created by LIUHAO on 2017/12/8.
//

#ifndef URHO3DCOPY_ABSTRACTFILE_H
#define URHO3DCOPY_ABSTRACTFILE_H

#include "Serializer.h"
#include "Deserializer.h"

namespace Urho3D
{
	class AbstractFile : public Deserializer, public Serializer
	{
	public:
		AbstractFile() : Deserializer(), Serializer() {}
		AbstractFile(unsigned int size) : Deserializer(size) {}
		virtual ~AbstractFile() override {}
	};
}



#endif //URHO3DCOPY_ABSTRACTFILE_H
