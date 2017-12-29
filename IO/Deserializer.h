//
// Created by LIUHAO on 2017/12/8.
//

#ifndef URHO3DCOPY_DESERIALIZER_H
#define URHO3DCOPY_DESERIALIZER_H

#include "../Container/Str.h"
#include "../Core/Variant.h"

namespace Urho3D
{
	class Deserializer
	{
	public:
		Deserializer();
		Deserializer(unsigned size);
		virtual ~Deserializer();

		virtual unsigned Read(void* dest, unsigned size) = 0;
		virtual unsigned Seek(unsigned position) = 0;
		virtual const String& GetName() const;
		virtual unsigned GetChecksum();
		virtual bool IsEof() const
		{
			return position_ >= size_;
		}

		unsigned SeekRelative(int delta);
		unsigned GetPosition() const { return position_; }
		unsigned Tell() const { return position_; }

		unsigned GetSize() const { return size_; }

		long long ReadInt64();
		int ReadInt();
		short ReadShort();
		signed char ReadByte();
		unsigned long long ReadUInt64();
		unsigned ReadUInt();
		unsigned short ReadUShort();
		unsigned char ReadUByte();
		bool ReadBool();
		float ReadFloat();
		double ReadDouble();

		//todo
		//IntRect ReadIntRect();

		String ReadString();
		String ReadFileID();

		Variant ReadVariant();
		Variant ReadVariant(VariantType type);
		VariantVector ReadVariantVector();
		VariantMap ReadVariantMap();

		String ReadLine();

	protected:
		unsigned position_;
		unsigned size_;
	};

}



#endif //URHO3DCOPY_DESERIALIZER_H
