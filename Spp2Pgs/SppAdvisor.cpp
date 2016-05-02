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

#include <afx.h>
#include <S2PGlobal.h>
#include <BgraFrame.h>
#include <strmif.h>
#include <algorithm>

#include "SppAdvisor.h"


SppAdvisor::SppAdvisor(ISubPicProviderAlfa *spp, BdViFormat format, BdViFrameRate frameRate, int from, int to, int offset) :
	format(format), frameRate(frameRate), from(from), to(to), offset(offset),
	spp(AssertArgumentNotNull(spp))
{
	this->ParseSubPicProvider();
}


SppAdvisor::~SppAdvisor()
{
}



void SppAdvisor::ParseSubPicProvider()
{
	using namespace std;

	BgraFrame buffer{ GetFrameSize(format) };
	auto spd = spp2pgs::DescribeTargetBuffer(&buffer);
	auto const& fps = spp2pgs::GetFramePerSecond(frameRate);
	CComPtr<IVobSubRectList> extent;

	HRESULT hr = spp->CreateRectList(&extent);
	if (FAILED(hr))
	{
		throw AvsInitException(AvsInitExceptionType::Unknown, hr);
	}

	auto cur = from >= 0 ? from : 0;
	auto p = spp->GetStartPosition(GetRefTimeOfFrame(cur, frameRate), fps);

	while (p && (to < 0 || cur < to))
	{
		REFERENCE_TIME const &b = spp->GetStart(p, fps);	//begin
		REFERENCE_TIME const &e = spp->GetStop(p, fps);	//end

		auto const& pN = spp->GetNext(p);
		spp->RenderEx(spd, b, fps, extent);
		bool const &a = spp->IsAnimated(p);

		this->sq.push_back(
			StsDesc{
			spp2pgs::GetFirstFrameFromRT(b, frameRate),
			cur = spp2pgs::GetFirstFrameFromRT(e, frameRate),
			a }
		);

		p = pN;
	}

	if (from == -1)
	{
		from = sq.size() > 0 ? (*sq.begin()).b : -1;
	}

	if (to == -1)
	{
		to = sq.size() > 0 ? (*sq.rbegin()).e : -1;
	}
}

int SppAdvisor::IsBlank(int index) const
{
	int const &iIndex = index - this->offset;
	if (iIndex < from || iIndex >= to) return 1;

	auto const& cmp = [](StsDesc xSeg, int value) { return xSeg.e <= value; };
	auto const& seg = std::lower_bound(sq.begin(), sq.end(), iIndex, cmp);

	if (seg == sq.end() || (*seg).b > iIndex) return 1;	//beyond the end or outside of segments

	if ((*seg).b <= iIndex && !(*seg).a) return 0;	//inside of a static segment

	return -1;
}

int SppAdvisor::IsIdentical(int index1, int index2) const
{
	int const &iIndex1 = index1 - this->offset;
	int const &iIndex2 = index2 - this->offset;

	int blank1 = -1, blank2 = -1;
	if (iIndex1 < from || iIndex1 >= to) blank1 = 1;
	if (iIndex2 < from || iIndex2 >= to) blank2 = 1;

	auto const& cmp = [](StsDesc xSeg, int value) { return xSeg.e <= value; };

	if (blank1 == 1)
	{
		return IsBlank(iIndex2);
	}
	else if (blank2 == 1)
	{
		return IsBlank(iIndex1);
	}
	else
	{
		auto const& seg1 = std::lower_bound(sq.begin(), sq.end(), iIndex1, cmp);
		if (seg1 == sq.end() || (*seg1).b > iIndex1) blank1 = 1;	//beyond the end or outside of segments
		if ((*seg1).b <= iIndex1 && !(*seg1).a) blank1 = 0;

		auto const& seg2 = std::lower_bound(sq.begin(), sq.end(), iIndex2, cmp);
		if (seg2 == sq.end() || (*seg2).b > iIndex2) blank2 = 1;
		if ((*seg2).b <= iIndex2 && !(*seg2).a) blank2 = 0;

		if (blank1 == 1 && blank2 == 1) return 1;	//[1, 1], both are blank img
		if (blank1 + blank2 == 1) return 0;	//[0, 1] or [1, 0], different
		if (blank1 == 0 && blank2 == 0 && seg1 == seg2)	//inside the same static segment
		{
			return 1;
		}
	}

	return -1;
}