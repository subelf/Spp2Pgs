/*----------------------------------------------------------------------------
* avs2pgs - Generates BluRay PG Stream from RGBA AviSynth scripts
* by Giton Xu <adm@subelf.net>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*----------------------------------------------------------------------------*/

#include "pch.h"
#include "Global.h"
#include "Win32StreamEx.h"

namespace avs2pgs
{
	using namespace std;

	unique_ptr<StreamEx> OpenTempFile(unsigned long long requireFreeSpace)
	{
		TCHAR tempPath[MAX_PATH];
		TCHAR tempFname[MAX_PATH];
		DWORD result = ::GetTempPath(MAX_PATH, tempPath);

		if (!result)
		{
			throw TempOutputException(TempOutputExceptionType::FetchTempOutputPathFailed, result);
		}

		ULARGE_INTEGER freeSpace;
		result = ::GetDiskFreeSpaceEx(tempPath, &freeSpace, nullptr, nullptr);

		if (!result)
		{
			throw TempOutputException(TempOutputExceptionType::QueryFreeSpaceFailed, result);
		}

		if (requireFreeSpace)
		{
			if (freeSpace.QuadPart < requireFreeSpace)
			{
				throw TempOutputException(TempOutputExceptionType::NotEnoughFreeSpace, result);
			}
		}

		result = ::GetTempFileName(tempPath, _T("a2p"), 0, tempFname);
		if (!result)
		{
			throw TempOutputException(TempOutputExceptionType::FetchTempFileNameFailed, result);
		}

		HANDLE tempFile = ::CreateFile(
			tempFname,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
			NULL
			);
		if (tempFile == INVALID_HANDLE_VALUE)
		{
			throw TempOutputException(TempOutputExceptionType::TempFileCreateFailed, result);
		}

		return unique_ptr<StreamEx>(new Win32StreamEx(tempFile, true, true, true, nullptr));
	}

	unsigned __int64 ReadBE(unsigned __int8 const * buffer, int index, unsigned __int8 length)
	{
		int tByteIndex = (index >> 3);
		int tShift = (length + index - (tByteIndex << 3));
		unsigned __int64 tMaskAll = -1;
		tMaskAll = ~(tMaskAll << length);

		unsigned __int64 tRet = 0;

		while ((tShift -= 8) >= 0)
		{
			unsigned __int64 tByte = buffer[tByteIndex++];
			tRet |= (tByte << tShift);
		}

		tRet |= (buffer[tByteIndex++] >> -tShift);
		return tRet & tMaskAll;
	}

	void WriteBE(unsigned __int8 * buffer, int index, unsigned __int8 length, unsigned __int64 value)
	{
		int tByteIndex = (index >> 3);
		int tShift = (length + index - (tByteIndex << 3));
		unsigned __int64 tMaskAll = -1;
		tMaskAll = ~(tMaskAll << length);

		unsigned __int8 tMask = 0;
		unsigned __int64 tValue = value & tMaskAll;

		while ((tShift -= 8) >= 0)
		{
			tMask = static_cast<unsigned __int8>(tMaskAll >> tShift);
			buffer[tByteIndex] &= ~tMask;
			buffer[tByteIndex] |= (tValue >> tShift);

			tByteIndex++;
		}

		tMask = static_cast<unsigned __int8>(tMaskAll << -tShift);
		buffer[tByteIndex] &= ~tMask;
		buffer[tByteIndex] |= (tValue << -tShift);
	}


}