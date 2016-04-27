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

#include <vcclr.h>

#include <FrameStreamAdvisor.h>

#include "IFrameStreamAdvisor.h"

namespace avs2pgs
{
	using namespace Avs2PgsNet;

	class ClrAdvisor final
		: public avs2pgs::FrameStreamAdvisor
	{
	public:
		ClrAdvisor(IFrameStreamAdvisor ^advisor) : advisorNet(AssertClrArgumentNotNull(advisor)) {}
		~ClrAdvisor() {}

		int IsBlank(int index) const { return advisorNet->IsBlank(index); }

		int IsIdentical(int index1, int index2) const { return advisorNet->IsIdentical(index1, index2); }

		int GetFirstPossibleImage() const { return advisorNet->FirstPossibleImage; }
		int GetLastPossibleImage() const { return advisorNet->LastPossibleImage; }

		avs2pgs::BdViFormat GetFrameFormat() const { return static_cast<avs2pgs::BdViFormat>(advisorNet->FrameFormat); }
		avs2pgs::BdViFrameRate GetFrameRate() const { return static_cast<avs2pgs::BdViFrameRate>(advisorNet->FrameRate); }

	private:
		gcroot<IFrameStreamAdvisor ^> advisorNet;
	};

}
