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

using namespace BluraySharp::Common;

namespace Spp2PgsNet
{

	public interface class IFrameStreamAdvisor
	{
		//Return value: 0 for Image, 1 for Blank, -1 for Unknown;
		int IsBlank(int index) = 0;

		//Return value: 0 for Same, 1 for Identical, -1 for Unknown;
		int IsIdentical(int index1, int index2) = 0;

		property int FirstPossibleImage { int get(); }
		property int LastPossibleImage { int get(); }
		property int FrameIndexOffset { int get(); }

		property BluraySharp::Common::BdViFormat FrameFormat{ BluraySharp::Common::BdViFormat get(); }
		property BluraySharp::Common::BdViFrameRate FrameRate{ BluraySharp::Common::BdViFrameRate get(); }
	};

}