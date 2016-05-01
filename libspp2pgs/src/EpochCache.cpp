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
#include "EpochCache.h"

namespace spp2pgs
{

	EpochCache::EpochCache(S2PContext const * context)
		:S2PControllerBase(context), tempFile(OpenTempFile(context->Settings()->MaxCachingSize()))
	{
	}

	EpochCache::~EpochCache()
	{
	}

	CacheReceipt EpochCache::Write(Rect dataCrop, IndexedImage *image)
	{		
		//Rect dataCrop = thumb->GetDataCrop();
		int imageSize = dataCrop.w * dataCrop.h;

		if (imageSize > 0 && !image->IsBlank())
		{
			CacheReceipt receipt(tempFile->GetPosition());
			
			int paletteSize = image->GetPaletteLength();
			Pixel32 *palette = image->GetPalette();
			char* imageData = image->GetDataBuffer();
			
			DWORD toWrite = sizeof(paletteSize);
			tempFile->Write(reinterpret_cast<byte*>(&paletteSize), 0, toWrite);

			toWrite = paletteSize * sizeof(Pixel32);
			tempFile->Write(reinterpret_cast<byte*>(palette), 0, toWrite);
			
			toWrite = sizeof(imageSize);
			tempFile->Write(reinterpret_cast<byte*>(&imageSize), 0, toWrite);

			int cropBottom = dataCrop.Bottom();
			toWrite = dataCrop.w;
			for (int y = dataCrop.Top(); y < cropBottom; ++y)
			{
				char* line = imageData + y * image->GetStride();
				tempFile->Write(reinterpret_cast<byte*>(line), dataCrop.x, toWrite);
			}

			return receipt;
		}
		else
		{
			return CacheReceipt(-1);
		}

	}

	void EpochCache::Load(CacheReceipt receipt, Rect dataCrop, IndexedImage *image)
	{
		auto pos = receipt.GetPosition();

		if (pos >= 0)
		{
			tempFile->SetPosition(pos);

			int paletteSize, imageSize;
			DWORD toRead = sizeof(paletteSize);
			tempFile->Read(reinterpret_cast<byte*>(&paletteSize), 0, toRead);
			image->SetPaletteLength(paletteSize);

			Pixel32 *palette = image->GetPalette();
			char* imageData = image->GetDataBuffer();

			toRead = paletteSize * sizeof(Pixel32);
			tempFile->Read(reinterpret_cast<byte*>(palette), 0, toRead);

			toRead = sizeof(imageSize);
			tempFile->Read(reinterpret_cast<byte*>(&imageSize), 0, toRead);
			
			assert(imageSize == dataCrop.Area());

			int cropBottom = dataCrop.Bottom();
			toRead = dataCrop.w;
			for (int y = dataCrop.Top(); y < cropBottom; ++y)
			{
				char* line = imageData + y * image->GetStride();
				tempFile->Read(reinterpret_cast<byte*>(line), dataCrop.x, toRead);
			}

			image->AnnounceDirty();
		}
		else
		{
			image->AnnounceBlank();
		}
	}

}