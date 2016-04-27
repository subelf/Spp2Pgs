/*----------------------------------------------------------------------------
* spp2pgs - Generates BluRay PG Stream from RGBA AviSynth scripts
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
#include "Win32StreamEx.h"

namespace spp2pgs
{

	Win32StreamEx::Win32StreamEx(HANDLE handle, bool canRead, bool canWrite, bool canSeek, TimeoutManager *timeoutManager)
		:handle(handle), canRead(canRead), canWrite(canWrite), canSeek(canSeek), timeoutManager(timeoutManager)
	{
		auto tType = ::GetFileType(handle);
		if (tType == FILE_TYPE_UNKNOWN)
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		this->canSeek = (tType == FILE_TYPE_DISK) ? canSeek : false;
	}

	__int64 Win32StreamEx::GetLength()
	{
		__int64 currentPosition = this->GetPosition();
		this->Seek(0, SeekOrigin::End);

		__int64 tRet = this->GetPosition();
		this->Seek(currentPosition, SeekOrigin::Begin);

		return tRet;
	}

	void Win32StreamEx::SetLength(__int64 length)
	{
		__int64 currentPosition = this->GetPosition();
		__int64 newPosition = (currentPosition > length) ? length : currentPosition;

		this->Seek(length, SeekOrigin::Begin);

		//long tRet = this->GetPosition();
		if (!::SetEndOfFile(handle))
		{
			this->Seek(currentPosition, SeekOrigin::Begin);
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}

		this->Seek(newPosition, SeekOrigin::Begin);
	}

	int Win32StreamEx::GetReadTimeout()
	{
		if (!this->CanTimeout())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		return this->timeoutManager->GetReadTimeout();
	}

	void Win32StreamEx::SetReadTimeout(int timeoutMiliseconds)
	{
		if (!this->CanTimeout())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		this->timeoutManager->SetReadTimeout(timeoutMiliseconds);
	}

	int Win32StreamEx::GetWriteTimeout()
	{
		if (!this->CanTimeout())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		return this->timeoutManager->GetWriteTimeout();
	}

	void Win32StreamEx::SetWriteTimeout(int timeoutMiliseconds)
	{
		if (!this->CanTimeout())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		this->timeoutManager->SetWriteTimeout(timeoutMiliseconds);
	}


	int Win32StreamEx::Read(unsigned char* buffer, int index, int length)
	{
		if (!this->CanRead())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		DWORD readLength = 0;
		if (!::ReadFile(handle, buffer + index, length, &readLength, nullptr))
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}

		return readLength;
	}

	int Win32StreamEx::WriteBuffered(unsigned char const * buffer, int index, int length)
	{
		if (!this->CanWrite())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		DWORD writtenLength = 0;
		if (!::WriteFile(handle, buffer + index, length, &writtenLength, nullptr))
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}

		return writtenLength;
	}


	int Win32StreamEx::ReadByte()
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

	void Win32StreamEx::WriteByte(unsigned char value)
	{
		unsigned char buffer = value;
		int tWrittenLen = this->WriteBuffered(&buffer, 0, 1);
		if (tWrittenLen == 0)
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}
	}

	void Win32StreamEx::CopyTo(StreamEx& destination)
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


	void Win32StreamEx::Close()
	{
		if (!::CloseHandle(handle))
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}
	}

	__int64 Win32StreamEx::Seek(__int64 offset, SeekOrigin origin)
	{
		if (!this->CanSeek())
		{
			throw StreamOperationException(StreamOperationExceptionType::InvalidOperation);
		}

		LARGE_INTEGER tOffset;
		tOffset.QuadPart = offset;

		LARGE_INTEGER tRet;

		if (!::SetFilePointerEx(handle, tOffset, &tRet, static_cast<DWORD>(origin)))
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}

		return tRet.QuadPart;
	}

	void Win32StreamEx::Flush() 
	{
		if (!::FlushFileBuffers(handle))
		{
			throw StreamOperationException(StreamOperationExceptionType::OperationFailed);
		}
	}
}