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
#include "BgraSubPicStream.h"

namespace spp2pgs
{
	BgraSubPicStream::BgraSubPicStream(ISubPicProviderAlfa *spp, FrameStreamAdvisor const *advisor):
		FrameStream(), spp(spp), advisor(AssertArgumentNotNull(advisor)),
		frameCount(advisor->GetLastPossibleImage() - advisor->GetFrameIndexOffset()),
		index(advisor->GetFirstPossibleImage() - advisor->GetFrameIndexOffset()),
		frameSize(spp2pgs::GetFrameSize(advisor->GetFrameFormat())),
		frameRate(advisor->GetFrameRate()),
		indexOffset(advisor->GetFrameIndexOffset())
	{
	}

	int BgraSubPicStream::GetNextFrame(StillImage * image)
	{
		auto bgraImage = dynamic_cast<BgraFrame *>(image);
		if (bgraImage == nullptr)
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImagePixelFormat);
		}

		int next = this->index + 1;

		if (next >= this->frameCount)
		{
			this->index = this->frameCount;
			bgraImage->AnnounceBlank();
			return -1;
		}

		bool const &isValid = (bgraImage->GetWidth() == this->frameSize.w) &&
			(bgraImage->GetHeight() == this->frameSize.h);
		if (!isValid)
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImageSize);
		}

		int const &isAnnouncedBlank = (advisor != nullptr) ? advisor->IsBlank(next + this->indexOffset) : -1;

		if (isAnnouncedBlank == 1)
		{
			bgraImage->AnnounceBlank();
		}
		else
		{
			CComPtr<IVobSubRectList> rects;
			HRESULT hr = spp->CreateRectList(&rects);

			if (SUCCEEDED(hr))
			{
				SubPicAlfaDesc spd = spp2pgs::DescribeTargetBuffer(bgraImage);
				auto const &rt = spp2pgs::GetRefTimeOfFrame(next, this->frameRate);
				double const &fps = spp2pgs::GetFramePerSecond(this->frameRate);

				bgraImage->Erase();
				hr = spp->RenderAlpha(spd, rt, fps, rects);
			}

			if (SUCCEEDED(hr))
			{
				bgraImage->AnnounceModified();
				bgraImage->AnnounceNormalized();
				if (isAnnouncedBlank == 0)
				{
					bgraImage->AnnounceDirty();
				}

				for (auto pos = rects->GetHeadPosition(); pos != NULL; )
				{
					auto const &rect = rects->GetNext(pos);
					bgraImage->RegisterRrawnRects(
						Rect{
						rect.left,
						rect.top,
						rect.right - rect.left,
						rect.bottom - rect.top,
					});
				}
			}
			else
			{
				bgraImage->AnnounceBlank();
				return -1;
			}
		}

		return (this->index = next) + this->indexOffset;
	}


	int BgraSubPicStream::SkipFrame(StillImage *image)
	{
		int next = this->index + 1;

		if (next >= this->frameCount)
		{
			this->index = this->frameCount;
			return -1;
		}

		bool const &isValid = (image->GetPixelSize() == BgraFrame::PixelSize) &&
			(image->GetWidth() == this->frameSize.w) &&
			(image->GetHeight() == this->frameSize.h);
		if (!isValid)
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImageSize);
		}

		return (this->index = next) + this->indexOffset;
	}
}