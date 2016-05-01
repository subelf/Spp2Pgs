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


namespace spp2pgs
{

	class S2PSettings
	{
	protected:
		S2PSettings() {};

	public:
		virtual ~S2PSettings() {};

		virtual int MaxImageBlockSize() const throw() = 0;
		virtual unsigned long long MaxCachingSize() const throw() = 0;
		virtual bool IsForcingEpochZeroStart() const throw() = 0;
		virtual bool IsForcingTmtCompat() const throw() = 0;
	};

	class S2PDefaultSettings
		: public S2PSettings
	{
	public:
		unsigned long long MaxCachingSize() const
		{
			return unsigned long long(1) << 32;
		}
		
		bool IsForcingEpochZeroStart() const
		{
			return true;
		}

		bool IsForcingTmtCompat() const
		{
			return false;
		}

		int MaxImageBlockSize() const
		{
			return 0;
		}
	};

}