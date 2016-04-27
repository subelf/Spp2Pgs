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

#include "A2PControllerBase.h"
#include "DisplaySetThumb.h"
#include "IndexedImage.h"
#include "GxBufferManager.h"

namespace avs2pgs
{
	class CompositionEncoder final
		: public A2PControllerBase,
		protected CompositionBuffer
	{
	public:
		CompositionEncoder(A2PContext const *context, WindowsDescriptor const *windows, GxBufferManager *bufMgr);
		~CompositionEncoder() {};

		CompositionBuffer const * Encode(DisplaySetThumb const * thumb, IndexedImage const * image);

	private:
		GxBufferManager * bufMgr;

		DisplaySetThumb const * displaySet;
		IndexedImage const * image;

		int renderDuration;

		void Split();
		void SplitWithoutCropping();

		struct EncodedPalette { unsigned __int8 i, y, cr, cb, a; };
		void EncodePalette();
		int EncodePaletteData(unsigned __int8 * buffer);

		void EncodeObjects();
		int EncodeImageData(int oid, unsigned __int8 * buffer, Size objSize);

		HashCode GetImageHash(Rect hashRect);
		HashCode GetPaletteHash();

		Rect GetMaxAvailableRect(int objId);

		bool IsImageBufferValid(int objId);

		static Position ArrangeRect(Rect content, Rect window, Size rectSize);

		static size_t const EncodedPaletteItemSize = sizeof(EncodedPalette);
	};

}