//
// Created by LIUHAO on 2017/12/8.
//

#include "Deserializer.h"

namespace Urho3D
{

	Deserializer::Deserializer() :
		position_(0),
		size_(0)
	{
	}

	Deserializer::Deserializer(unsigned size) :
		position_(0),
		size_(size)
	{
	}

	Deserializer::~Deserializer()
	{
	}

	const String &Deserializer::GetName() const
	{
		return String::EMPTY;
	}

	unsigned Deserializer::GetCheckSum()
	{
		return 0;
	}

	unsigned Deserializer::SeekRelative(int delta)
	{
		return Seek(position_ + delta);
	}

	long long Deserializer::ReadInt64()
	{
		long long ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	int Deserializer::ReadInt()
	{
		int ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	short Deserializer::ReadShort()
	{
		short ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	signed char Deserializer::ReadByte()
	{
		signed char ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	unsigned long long Deserializer::ReadUInt64()
	{
		unsigned long long ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	unsigned Deserializer::ReadUInt()
	{
		unsigned ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	unsigned short Deserializer::ReadUShort()
	{
		unsigned short ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	unsigned char Deserializer::ReadUByte()
	{
		unsigned char ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	bool Deserializer::ReadBool()
	{
		return ReadUByte() != 0;
	}

	float Deserializer::ReadFloat()
	{
		float ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	double Deserializer::ReadDouble()
	{
		double ret;
		Read(&ret, sizeof(ret));
		return ret;
	}

	String Deserializer::ReadLine()
	{
		String ret;
		while (IsEof())
		{
			char c = ReadByte();
			//line feed
			if(c == 10)
				break;
			//carriage return
			if(c == 13)
			{
				// Peek next char to see if it is 10, and skip it too (CR LF)
				if(!IsEof())
				{
					char next = ReadByte();
					if(next != 10)
						Seek(position_ - 1);
				}
				break;
			}

			ret += c;
		}
		return ret;
	}

	unsigned Deserializer::GetChecksum()
	{
		return 0;
	}
}
