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

#pragma once

namespace spp2pgs
{
	enum class SeekOrigin
	{
		Begin = 0,
		Current = 1,
		End = 2,
	};

	class StreamEx
	{
	protected:
		StreamEx() {};

	public:
		virtual ~StreamEx() {};

		virtual bool CanRead() = 0;
		virtual bool CanWrite() = 0;
		virtual bool CanSeek() = 0;
		virtual bool CanTimeout() = 0;

		virtual __int64 GetLength() = 0;
		virtual void SetLength(__int64 length) = 0;
		virtual __int64 GetPosition() = 0;
		virtual void SetPosition(__int64 position) = 0;

		virtual int GetReadTimeout() = 0;
		virtual void SetReadTimeout(int timeoutMiliseconds) = 0;
		virtual int GetWriteTimeout() = 0;
		virtual void SetWriteTimeout(int timeoutMiliseconds) = 0;

		virtual int Read(unsigned char* buffer, int index, int length) = 0;
		virtual int WriteBuffered(unsigned char const* buffer, int index, int length) = 0;
		inline void Write(unsigned char const* buffer, int index, int length)
		{
			int tWrittenLen = 0;
			int tIndex = index;
			int tLeft = length;
			for (;;)
			{
				tWrittenLen = this->WriteBuffered(buffer, tIndex, tLeft);
				tIndex += tWrittenLen;
				tLeft -= tWrittenLen;

				if (tLeft <= 0)
				{
					break;
				}
			}
		}

		virtual int ReadByte() = 0;
		virtual void WriteByte(unsigned char const value) = 0;

		virtual void CopyTo(StreamEx& destination) = 0;

		virtual void Close() = 0;

		virtual __int64 Seek(__int64 offset, SeekOrigin origin) = 0;
		virtual void Flush() = 0;
	};

	class TimeoutManager
	{
	protected:
		TimeoutManager() {};

	public:
		virtual ~TimeoutManager() = 0;

		virtual bool CanTimeout() = 0;
		virtual int GetReadTimeout() = 0;
		virtual void SetReadTimeout(int timeoutMiliseconds) = 0;
		virtual int GetWriteTimeout() = 0;
		virtual void SetWriteTimeout(int timeoutMiliseconds) = 0;

	};

	inline TimeoutManager::~TimeoutManager() {}

}