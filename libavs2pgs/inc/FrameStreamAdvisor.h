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

#include "BlurayCommon.h"

namespace avs2pgs
{

	class FrameStreamAdvisor
	{
	public:
		FrameStreamAdvisor() {};
		virtual ~FrameStreamAdvisor() {};

		//Return value: 0 for Image, 1 for Blank, -1 for Unknown;
		virtual int IsBlank(int index) const = 0;

		//Return value: 0 for Same, 1 for Identical, -1 for Unknown;
		virtual int IsIdentical(int index1, int index2) const = 0;

		virtual int GetFirstPossibleImage() const = 0;
		virtual int GetLastPossibleImage() const = 0;

		virtual BdViFormat GetFrameFormat() const = 0;
		virtual BdViFrameRate GetFrameRate() const = 0;
	};

}