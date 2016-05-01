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

using namespace spp2pgs;

class S2PClaSettings final:
	public S2PSettings
{
public:
	S2PClaSettings() {}
	~S2PClaSettings() {}

public:
	unsigned long long MaxCachingSize() const throw() { return maxCachingSize; }
	int MaxImageBlockSize() const throw() { return maxImageBlockSize; }
	bool IsForcingEpochZeroStart() const throw() { return isForcingEpochZeroStart; }
	bool IsForcingTmtCompat() const throw() { return isForcingTmtCompat; }

	void SetMaxCachingSize(unsigned long long maxCachingSize) { this->maxCachingSize = maxCachingSize; }
	void SetForcingEpochZeroStart(bool flag) { this->isForcingEpochZeroStart = flag; }
	void SetForcingTmtCompat(bool flag) { this->isForcingTmtCompat = flag; }

private:
	unsigned long long maxCachingSize = unsigned long long(1) << 32;
	int maxImageBlockSize = 0;
	bool isForcingEpochZeroStart = true;
	bool isForcingTmtCompat = false;
};

