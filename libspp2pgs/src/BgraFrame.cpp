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
#include "BgraFrame.h"
#include "Extern.h"

namespace spp2pgs
{

	BgraFrame::BgraFrame(Size frameSize)
		: StillImage(frameSize, BgraFrame::PixelSize, frameSize.w * BgraFrame::PixelSize), index(-1)
	{
	}


	BgraFrame::~BgraFrame()
	{
	}


	bool BgraFrame::IsIdenticalTo(BgraFrame *frame)
	{
		if (frame == nullptr)
		{
			return false;
		}

		if (this->index == frame->index)
		{
			return true;
		}
		else if (this->index == -1 || frame->index == -1)
		{
			return false;
		}

		if (this->advisor != nullptr && this->advisor == frame->advisor)
		{
			int index1 = this->GetFrameIndex();
			int index2 = frame->GetFrameIndex();

			int isIdentical = advisor->IsIdentical(index1, index2);
			if (isIdentical != -1)
			{
				return isIdentical != 0;
			}
		}

		return StillImage::IsIdenticalTo(frame);
	}

	bool BgraFrame::IsBlank()
	{
		if (this->index == -1)
		{
			return true;
		}

		if (this->advisor != nullptr)
		{
			int index = this->GetFrameIndex();

			int isBlank = advisor->IsBlank(index);
			if (isBlank != -1)
			{
				return isBlank != 0;
			}
		}

		return StillImage::IsBlank();
	}

	bool BgraFrame::IsExplicitIdenticalTo(StillImage const *image) const
	{
		//_tprintf(_T("asm_is_identical_sse2\n"));
		return (asm_is_identical_sse2(this->GetDataSize(), this->GetDataBuffer(), image->GetDataBuffer()) != 0);
	}

	bool BgraFrame::IsExplicitBlank() const
	{
		//_tprintf(_T("asm_is_empty_sse2\n"));
		return (asm_is_empty_sse2(this->GetDataSize(), this->GetDataBuffer()) != 0);
	}

	void BgraFrame::ExplicitEraseTransparents() const
	{
		//_tprintf(_T("asm_zero_transparent_sse2\n"));
		asm_zero_transparent_sse2(this->GetDataSize(), this->GetDataBuffer());
	}

	void BgraFrame::ExplicitErase(Rect rect) const
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