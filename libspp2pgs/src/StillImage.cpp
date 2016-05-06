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
#include "StillImage.h"

namespace spp2pgs
{
	StillImage::StillImage(Size imageSize, int pixelSize, int stride)
		:imageSize(imageSize), isBlank(-1), pixelSize(pixelSize),
		stride((stride >= this->imageSize.w * pixelSize) ? stride : (stride == 0 ? AlignTo(this->imageSize.w * pixelSize, Align32) : (this->imageSize.w * pixelSize)))
	{	
		this->dataSize = this->stride * this->imageSize.h;
		auto alignedSize = AlignTo(this->dataSize, Align32);

		this->buffer = new char[alignedSize + Align32];
		this->dataBuffer = AlignTo(this->buffer, Align32);	//Align to 256bit for SSE
		
		//while (alignedSize > this->dataSize) dataBuffer[--alignedSize] = 0;
		memset(dataBuffer + this->dataSize, 0, (alignedSize - this->dataSize));	//Erease padding bytes
	}


	StillImage::~StillImage()
	{
		if (this->buffer != nullptr)
		{
			delete[] this->buffer;
			this->buffer = nullptr;
		}
	}

	bool StillImage::IsIdenticalTo(StillImage *image)
	{
		bool isIdentical = this->GetPixelSize() == image->GetPixelSize();
		isIdentical &= this->GetStride() == image->GetStride();
		isIdentical &= this->GetHeight() == image->GetHeight();

		if (!isIdentical) return isIdentical;

		if (image->isBlank == 1)
		{
			return this->IsBlank();
		}
		else if (this->isBlank == 1)
		{
			return image->IsBlank();
		}
		else
		{
			if (this->isNormalized)
			{
				isIdentical = image->IsExplicitIdenticalTo(this);
			}
			else
			{
				image->Normalize();
				isIdentical = this->IsExplicitIdenticalTo(image);
			}
			return isIdentical;
		}
	}

	bool StillImage::IsBlank() const
	{
		if (this->isBlank == -1)
		{
			return IsExplicitBlank();
		}

		return (this->isBlank != 0);
	}

	void StillImage::Erase()
	{
		if (this->IsBlank() == 1 && this->isExplicitErased)
		{
			return;
		}

		if(this->drawnRects.empty())
		{
			this->ExplicitErase(Rect{ 0, 0, imageSize });
			AnnounceBlank();
			AnnounceNormalized();
		}
		else
		{
			for (auto const& iRect : this->drawnRects)
			{
				this->ExplicitErase(iRect);
			}
			
			InvalidateRrawnRects();
			AnnounceBlank();
			AnnounceNormalized();
		}

		this->isExplicitErased = true;
	}

	void StillImage::Normalize()
	{
		if (this->isNormalized)
			return;

		ExplicitEraseTransparents();
		//InvalidateRrawnRects();
		this->isNormalized = true;
	}
}