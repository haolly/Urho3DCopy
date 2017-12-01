//
// Created by liuhao on 2017/11/28.
//

#include <cctype>
#include "Str.h"

namespace Urho3D
{
	int String::Compare(const char *str1, const char *str2, bool caseSenstive)
	{
		if (caseSenstive)
		{
			return strcmp(str1, str2);
		}
		if (!str1 || !str2)
		{
			return str1 ? 1 : (str2 ? -1 : 0);
		}
		int pos = 0;
		do
		{
			char a = static_cast<char>(tolower(str1[pos]));
			char b = static_cast<char>(tolower(str2[pos]));
			if (!a || !b)
			{
				return a ? 1 : (b ? -1 : 0);
			}
			if (a == b)
			{
				++pos;
			}
			if (a < b)
			{
				return -1;
			}
			if (a > b)
			{
				return 1;
			}
		}
		while (true);
	}
}