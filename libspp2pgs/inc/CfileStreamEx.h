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

#include <cstdio>

#include "StreamEx.h"

namespace spp2pgs
{
	class CfileStreamEx
		: public StreamEx
	{
	public:
		CfileStreamEx(FILE* cfile, bool canRead, bool canWrite, bool canSeek, TimeoutManager *timeoutManager);
		~CfileStreamEx() { }

		bool CanRead() { return this->canRead; }
		bool CanWrite(){ return this->canWrite; }
		bool CanSeek(){ return this->canSeek; }
		bool CanTimeout(){ return (this->timeoutManager != nullptr) && (this->timeoutManager->CanTimeout()); }

		__int64 GetLength();
		void SetLength(__int64 length);
		__int64 GetPosition() { return this->Seek(0, SeekOrigin::Current); }
		void SetPosition(__int64 position) { this->Seek(position, SeekOrigin::Begin); }

		int GetReadTimeout();
		void SetReadTimeout(int timeoutMiliseconds);
		int GetWriteTimeout();
		void SetWriteTimeout(int timeoutMiliseconds);

		int Read(unsigned char * buffer, int index, int length);
		int WriteBuffered(unsigned char const * buffer, int index, int length);

		int ReadByte();
		void WriteByte(unsigned char const value);

		void CopyTo(StreamEx& destination);

		void Close();

		__int64 Seek(__int64 offset, SeekOrigin origin);
		void Flush();

	private:
		FILE* file;
		bool canRead, canWrite, canSeek;
		TimeoutManager *timeoutManager;
	};

}