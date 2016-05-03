/*----------------------------------------------------------------------------
* spp2pgs - Generates BluRay PG Stream from Subtitles or AviSynth scripts
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

#pragma once

#include <vcclr.h>

#include <S2PExceptions.h>

#include "StreamEx.h"


namespace spp2pgs
{

	using namespace System;
	using namespace System::IO;
	using namespace Spp2PgsNet;

	class ClrStreamEx final
		: public spp2pgs::StreamEx
	{
	public:
		ClrStreamEx(Stream ^ baseStream) : baseStream(AssertClrArgumentNotNull(baseStream)) {}

		inline bool CanRead() { return this->baseStream->CanRead; }
		inline bool CanWrite(){ return this->baseStream->CanWrite; }
		inline bool CanSeek(){ return this->baseStream->CanSeek; }
		inline bool CanTimeout(){ return this->baseStream->CanTimeout; }

		inline __int64 GetLength() { return this->baseStream->Length; }
		inline void SetLength(__int64 length) { this->baseStream->SetLength(length); }
		inline __int64 GetPosition() { return this->baseStream->Position; }
		inline void SetPosition(__int64 position) { this->baseStream->Position = position; }

		inline int GetReadTimeout() { return this->baseStream->ReadTimeout; }
		inline void SetReadTimeout(int timeoutMiliseconds) { this->baseStream->ReadTimeout = timeoutMiliseconds; }
		inline int GetWriteTimeout() { return this->baseStream->WriteTimeout; }
		inline void SetWriteTimeout(int timeoutMiliseconds) { this->baseStream->WriteTimeout = timeoutMiliseconds; }

		inline int Read(unsigned char * buffer, int index, int length) {
			int const &tBufferLen = min(length, BufferLength);
			array<Byte> ^tBuffer = gcnew array<Byte>(tBufferLen);
			auto tDest = IntPtr(const_cast<Byte*>(buffer + index));
			auto tLeft = length;

			while (tLeft > 0)
			{
				int const &tLen = min(tLeft, tBufferLen);
				int const &tLenRead = this->baseStream->Read(tBuffer, 0, tLen);
				if (tLenRead <= 0) break;
				System::Runtime::InteropServices::Marshal::Copy(tBuffer, 0, tDest, tLenRead);
				tLeft -= tLenRead; tDest += tLenRead;
			}

			return length - tLeft;
		}
		inline int WriteBuffered(unsigned char const * buffer, int index, int length) {
			int const &tBufferLen = min(length, BufferLength);
			array<Byte> ^tBuffer = gcnew array<Byte>(tBufferLen);
			auto tSource = IntPtr(const_cast<Byte*>(buffer + index));
			auto tLeft = length;

			while (tLeft > 0)
			{
				int const &tLen = min(tLeft, tBufferLen);
				System::Runtime::InteropServices::Marshal::Copy(tSource, tBuffer, 0, tLen);
				this->baseStream->Write(tBuffer, 0, tLen);
				tLeft -= tLen; tSource += tLen;
			}

			return length;
		}

		inline int ReadByte() { return this->baseStream->ReadByte(); }
		inline void WriteByte(unsigned char const value) { this->baseStream->WriteByte(value); }

		inline void CopyTo(StreamEx& destination){
			array<Byte> ^buffer = gcnew array<Byte>(BufferLength);
			pin_ptr<Byte> umBuffer = &buffer[0];

			int tReadLen = 0;
			for (;;)
			{
				tReadLen = this->baseStream->Read(buffer, 0, BufferLength);
				if (tReadLen == 0)
				{
					break;
				}

				destination.Write(umBuffer, 0, tReadLen);
			}
		}

		inline void Close() { this->baseStream->Close(); }

		__int64 Seek(__int64 offset, spp2pgs::SeekOrigin origin) {
			return this->baseStream->Seek(offset, static_cast<System::IO::SeekOrigin>(origin));
		}
		void Flush() { this->baseStream->Flush(); }

	private:
		static int const BufferLength = 64 * 1024 * 1024;
		gcroot<Stream ^> baseStream;
	};

}