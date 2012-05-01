/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#include "Platform.h"
#include <iostream>

namespace Platform {

gs2d::str_type::string GetFileName(const gs2d::str_type::string &source)
{
	const unsigned int len = source.length();
	gs2d::str_type::string r = source;
	unsigned int t;
	for (t = len-1; t > 0; t--)
	{
		if (r[t] == GS_L('\\') || r[t] == GS_L('/'))
		{
			r = r.substr(t+1);
			break;
		}
	}
	return r;
}

std::string GetFilePath(const char *source)
{
	std::string dest = source;
	const int len = strlen(source);
	for (int t=len; t>0; t--)
	{
		if (source[t] == '/' || source[t] == '\\')
		{
			dest.resize(t + 1);
			break;
		}
	}
	return dest;
}

} // namespace Platform