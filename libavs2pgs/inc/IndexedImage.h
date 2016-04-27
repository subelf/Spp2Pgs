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

#include "A2PExceptions.h"
#include "StillImage.h"
#include "GraphicalTypes.h"
#include "Extern.h"

namespace avs2pgs
{

	class IndexedImage : public StillImage
	{
	public:
		IndexedImage(Size frameSize);
		IndexedImage(StillImage* bgraImg, Rect validRect);
		~IndexedImage();

		void ImportFrom(StillImage* src, Rect dataCrop);
		inline int GetPaletteLength() const { return paletteLength; }
		inline void SetPaletteLength(int length) {
			if (length > Colors + 1) throw ImageOperationException(ImageOperationExceptionType::PaletteSizeOverflow); paletteLength = length;
		}
		inline Pixel32 * GetPalette() const { return palette; }

	protected:
		bool IsExplicitIdenticalTo(StillImage const *image) const override;
		bool IsExplicitBlank() const override;
		void ExplicitEraseTransparents() const override;
		void ExplicitErase(Rect rect) const override;

	private:
		static int const PixelSize = 1;

		Pixel32 *palette;
		int paletteLength;
	};

}