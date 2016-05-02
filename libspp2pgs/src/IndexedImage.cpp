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

#include "pch.h"
#include "IndexedImage.h"

namespace spp2pgs
{

	IndexedImage::IndexedImage(Size frameSize)
		:StillImage(frameSize, PixelSize, -1), palette(new Pixel32[Colors + 1])
	{
	}

	IndexedImage::IndexedImage(StillImage *bgraImg, Rect dataCrop)
		: StillImage(bgraImg->GetImageSize(), PixelSize, -1), palette(new Pixel32[Colors + 1])
	{
		ImportFrom(bgraImg, dataCrop);
	}


	IndexedImage::~IndexedImage()
	{
		if (palette != nullptr)
		{
			delete[] palette;
			palette = nullptr;
		}
	}

	void IndexedImage::ImportFrom(StillImage* src, Rect dataCrop)
	{
		if (!dataCrop.w || !dataCrop.h)
		{
			this->AnnounceBlank();
			return;
		}

		char* srcImg = src->GetDataBuffer();
		char* destImg = this->GetDataBuffer();
		quantizer_t *q = new_quantizer();

		int x, y;
		Border b = dataCrop.ToOuterBorder();
		int ws = src->GetStride();
		int wd = this->GetStride();

		for (y = b.top; y < b.bottom; y++)
		{
			Pixel32* line = (Pixel32*)(srcImg + y * ws);
			for (x = b.left; x < b.right; x++)
				insert_color(q, line[x]);
		}

		paletteLength = get_palette(q, palette);

		for (y = b.top; y < b.bottom; y++)
		{
			Pixel32* srcLine = (Pixel32*)(srcImg + y * ws);
			char* destLine = destImg + y * wd;
			for (x = b.left; x < b.right; x++)
				destLine[x] = get_color_index(q, srcLine[x]);
		}

		destroy_quantizer(q);

		this->AnnounceDirty(); 
		this->AnnounceNormalized();
	}

	bool IndexedImage::IsExplicitIdenticalTo(StillImage const *image) const
	{
		throw S2PException(S2PExceptionType::NotImplenmented, nullptr);
	}

	bool IndexedImage::IsExplicitBlank() const
	{
		char* tImage = this->GetDataBuffer();

		Pixel32* tPal = this->GetPalette();
		int tPalLen = this->GetPaletteLength();

		if (tPalLen == 0) return true;
		if (tPal[0].a != 0) return false;

		Size b = this->GetImageSize();
		for (int y = 0; y < b.h; ++y)
		{
			char* tLine = tImage + y * this->GetStride();
			for (int x = 0; x < b.w; ++x)
			{
				if (tLine[x]) return false;
			}
		}

		return true;
	}

	void IndexedImage::ExplicitEraseTransparents() const
	{
		throw S2PException(S2PExceptionType::NotImplenmented, nullptr);
	}

	void IndexedImage::ExplicitErase(Rect rect) const
	{
		char* tImage = this->GetDataBuffer();
		int tBottom = rect.Bottom();
		int tOffset = rect.x * PixelSize;
		int tWidth = rect.w * PixelSize;

		for (int y = rect.Top(); y < tBottom; ++y)
		{
			char* tLine = tImage + y * this->GetStride();
			::memset(tLine + tOffset, 0, tWidth);
		}
	}
}