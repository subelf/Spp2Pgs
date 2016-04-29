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

#include <afx.h>
#include <ISubPic.h>
#include <strmif.h>
#include <vector>
#include <FrameStreamAdvisor.h>

using namespace spp2pgs;

class SppAdvisor:
	public FrameStreamAdvisor
{
public:
	SppAdvisor(ISubPicProvider *spp, BdViFormat format, BdViFrameRate frameRate, int from, int to);
	~SppAdvisor();

	int IsBlank(int index) const;
	int IsIdentical(int index1, int index2) const;

	int GetFirstPossibleImage() const { return from; }
	int GetLastPossibleImage() const { return to; }

	BdViFormat GetFrameFormat() const { return format; }
	BdViFrameRate GetFrameRate() const { return frameRate; }

private:
	BdViFormat format;
	BdViFrameRate frameRate;
	int from, to;

	struct StsDesc	//Subtitle Segment Descriptor
	{
		int b, e;	//begin, end
		bool a;	//is Animated
	};

	CComPtr<ISubPicProvider> spp;
	std::vector<StsDesc> sq;

	void ParseSubPicProvider();
};

