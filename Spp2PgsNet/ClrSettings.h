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

#pragma once

#include <A2PSettings.h>

#include "IA2PSettings.h"


namespace avs2pgs
{

	using namespace Avs2PgsNet;

	class ClrSettings final :
		public A2PSettings
	{
	public:
		ClrSettings(IA2PSettings ^settings) : settingsNet(AssertClrArgumentNotNull(settings)) {}
		~ClrSettings() {}

	public:
		unsigned long long MaxCachingSize() const throw() { return settingsNet->MaxCachingSize; }
		int MaxImageBlockSize() const throw() { return settingsNet->MaxImageBlockSize; }
		bool IsForcingEpochZeroStart() const throw() { return settingsNet->IsForcingEpochZeroStart; }
		bool IsForcingTmtCompat() const throw() { return settingsNet->IsForcingTmtCompat; }

	private:
		gcroot<IA2PSettings ^> settingsNet;
	};

}