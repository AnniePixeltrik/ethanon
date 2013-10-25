/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#ifndef FILE_LISTING_H_
#define FILE_LISTING_H_

#include <Platform/Platform.h>

#include <vector>

#include <iostream>

namespace Platform {

class FileListing
{
public:
	struct FILE_NAME
	{
		inline void GetFullFileName(gs2d::str_type::string &sOut)
		{
			sOut = dir + file;
		}
		gs2d::str_type::string dir;
		gs2d::str_type::string file;
	};

	bool ListDirectoryFiles(const gs2d::str_type::char_t* directory, const gs2d::str_type::char_t* extension);

	inline unsigned int GetNumFiles()
	{
		return static_cast<unsigned int>(m_fileName.size());
	}

	inline bool GetFileName(const unsigned int index, FILE_NAME& fileName)
	{
		if (index >= GetNumFiles())
		{
			GS2D_CERR << GS_L("FileListing::GetFileName - index >= m_fileName.size().\n");
			return false;
		}
		fileName = m_fileName[index];
		return true;
	}

private:
	std::vector<FILE_NAME> m_fileName;
};

} // namespace Platform

#endif