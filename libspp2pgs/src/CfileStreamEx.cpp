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

#include <io.h>
#include <fcntl.h>
#include "CFileStreamEx.h"


namespace avs2pgs
{

	CfileStreamEx::CfileStreamEx(FILE* cfile, bool canRead, bool canWrite, bool canSeek, TimeoutManager *timeoutManager)
		:file(cfile), canRead(canRead), canWrite(canWrite), canSeek(canSeek), timeoutManager(timeoutManager)
	{
		_setmode(_fileno(file), _O_BINARY);
	}

	__int64 CfileStreamEx::GetLength()
	{
		auto tLen = _filelengthi64(_fileno(file));
		if (tLen == -1)
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}

		return tLen;
	}

	void CfileStreamEx::SetLength(__int64 length)
	{
		auto tErr = _chsize_s(_fileno(file), length);
		if (tErr)
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}
	}

	int CfileStreamEx::GetReadTimeout()
	{
		if (!this->CanTimeout())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		return this->timeoutManager->GetReadTimeout();
	}

	void CfileStreamEx::SetReadTimeout(int timeoutMiliseconds)
	{
		if (!this->CanTimeout())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		this->timeoutManager->SetReadTimeout(timeoutMiliseconds);
	}

	int CfileStreamEx::GetWriteTimeout()
	{
		if (!this->CanTimeout())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		return this->timeoutManager->GetWriteTimeout();
	}

	void CfileStreamEx::SetWriteTimeout(int timeoutMiliseconds)
	{
		if (!this->CanTimeout())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		this->timeoutManager->SetWriteTimeout(timeoutMiliseconds);
	}


	int CfileStreamEx::Read(unsigned char* buffer, int index, int length)
	{
		if (!this->CanRead())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}
		size_t readLength = 0;
		readLength = fread(buffer + index, 1, length, file);

		return static_cast<int>(readLength);
	}

	int CfileStreamEx::WriteBuffered(unsigned char const * buffer, int index, int length)
	{
		if (!this->CanWrite())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}
		size_t writtenLength = 0;
		writtenLength = fwrite(buffer + index, 1, length, file);

		return static_cast<int>(writtenLength);
	}


	int CfileStreamEx::ReadByte()
	{
		unsigned char buffer;
		int tReadLen = this->Read(&buffer, 0, 1);
		if (tReadLen == 0)
		{
			return -1;
		}
		else
		{
			return static_cast<int>(buffer);
		}
	}

	void CfileStreamEx::WriteByte(unsigned char value)
	{
		unsigned char buffer = value;
		int tWrittenLen = this->WriteBuffered(&buffer, 0, 1);
		if (tWrittenLen == 0)
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}
	}

	void CfileStreamEx::CopyTo(StreamEx& destination)
	{
		const int BufferLength = 64 * 1024 * 1024;
		unsigned char buffer[BufferLength];

		int tReadLen = 0;
		for (;;)
		{
			tReadLen = this->Read(buffer, 0, BufferLength);
			if (tReadLen == 0)
			{
				break;
			}

			destination.Write(buffer, 0, tReadLen);
		}
	}


	void CfileStreamEx::Close()
	{
		if (fclose(file))
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}
	}

	__int64 CfileStreamEx::Seek(__int64 offset, SeekOrigin origin)
	{
		if (!this->CanSeek())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		auto tRet = _fseeki64(file, offset, static_cast<int>(origin));
			
		return tRet;
	}

	void CfileStreamEx::Flush()
	{
		if (fflush(file))
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}
	}
}