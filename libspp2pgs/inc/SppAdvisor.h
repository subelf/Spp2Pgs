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

#include <afx.h>
#include <strmif.h>
#include <vector>
#include <VSSppfApi.h>

#include "SimpleAdvisor.h"
#include "ProgressReporter.h"

using namespace spp2pgs;

class SppAdvisor:
	public SimpleAdvisor
{
public:
	SppAdvisor(ISubPicProviderAlfa *spp, BdViFormat format, BdViFrameRate frameRate, int from, int to, int offset = 0, ProgressReporter *reporter=nullptr);

	int IsBlank(int index) const;
	int IsIdentical(int index1, int index2) const;

	int GetFirstPossibleImage() const { return from + offset; }
	int GetLastPossibleImage() const { return to + offset; }
	int GetFrameIndexOffset() const { return offset; }

	BdViFormat GetFrameFormat() const { return format; }
	BdViFrameRate GetFrameRate() const { return frameRate; }

private:

	struct StsDesc	//Subtitle Segment Descriptor
	{
		int b, e;	//begin, end
		bool a;	//is Animated
	};

	CComPtr<ISubPicProviderAlfa> spp;
	std::vector<StsDesc> sq;

	void ParseSubPicProvider(ProgressReporter *reporter);
};

