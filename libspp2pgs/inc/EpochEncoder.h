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

#include "GraphicalTypes.h"
#include "DisplaySetThumb.h"
#include "CompositionEncoder.h"

namespace spp2pgs
{

	class EpochEncoder final
		: public S2PControllerBase
	{
	public:
		EpochEncoder(S2PContext const *context, GxBufferManager * bufMgr, BinaryBoard const * binBoard, Size const & size);
		~EpochEncoder();

		CompositionBuffer const * EncodeComposition(DisplaySetThumb const * thumb, IndexedImage const * image);
		inline WindowsDescriptor const * GetWindowsDescriptor() { return &windows; }

	private:
		WindowsDescriptor windows;
		GxBufferManager * bufMgr;

		static WindowsDescriptor CreateWindows(BinaryBoard const * binBoard);
	};

}