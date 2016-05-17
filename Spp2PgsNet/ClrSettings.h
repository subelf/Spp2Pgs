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

#include <S2PSettings.h>

#include "IS2PSettings.h"


namespace spp2pgs
{
	using namespace Spp2PgsNet;

	class ClrSettings final :
		public S2PSettings
	{
	public:
		ClrSettings(IS2PSettings ^settings) : settingsNet(AssertClrArgumentNotNull(settings)) {}
		~ClrSettings() {}

	public:
		unsigned long long MaxCachingSize() const throw() { return settingsNet->MaxCachingSize; }
		int MaxImageBlockSize() const throw() { return settingsNet->MaxImageBlockSize; }
		bool IsForcingTmtCompat() const throw() { return settingsNet->IsForcingTmtCompat; }
		TCHAR const * TempOutputPath() const throw() {
			auto const &tPath = settingsNet->TempOutputPath;
			if (String::IsNullOrWhiteSpace(tPath))
			{
				return nullptr;
			}

			auto tThis = const_cast<ClrSettings *>(this);
			pin_ptr<const TCHAR> tPinPath = PtrToStringChars(tPath);
			tThis->pTempOutputPath = tPinPath;

			return pTempOutputPath.c_str();
		}

	private:
		gcroot<IS2PSettings ^> settingsNet;
		_tstring pTempOutputPath;

	};

}
