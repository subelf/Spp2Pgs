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
#include "BgraRawStream.h"

#include "BgraFrame.h"

namespace spp2pgs
{

	BgraRawStream::BgraRawStream(StreamEx * stream, FrameStreamAdvisor const * advisor)
		:input(stream), advisor(advisor), index(-1), readIndex(-1), eos(false)
	{
		if (stream == nullptr || advisor == nullptr)
		{
			throw S2PException(S2PExceptionType::ArgumentNull, nullptr);
		}

		this->frameSize = spp2pgs::GetFrameSize(advisor->GetFrameFormat());
		this->frameRate = advisor->GetFrameRate();
		this->frameCount = advisor->GetLastPossibleImage();
		this->index = advisor->GetFirstPossibleImage() - 1;

		if (this->frameSize.Area() == 0 || this->frameRate == BdViFrameRate::Unknown)
		{
			throw S2PException(S2PExceptionType::Unknown, nullptr);
		}

		this->index = (this->index < 0) ? -1 : this->index;
	}

	int BgraRawStream::GetNextFrame(StillImage *image)
	{
		auto bgraImage = dynamic_cast<BgraFrame *>(image);
		if (bgraImage == nullptr)
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImagePixelFormat);
		}

		int next = this->index + 1;
		SkipFrames((unsigned __int8*)(image->GetDataBuffer()));

		if (eos || (this->frameCount != -1 && next >= this->frameCount))
		{
			eos = true;
			image->AnnounceBlank();
			return -1;
		}

		bool const &isValid = (bgraImage->GetWidth() == this->frameSize.w) &&
			(bgraImage->GetHeight() == this->frameSize.h);
		if (!isValid)
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImageSize);
		}

		int const &isAnnouncedBlank = (advisor != nullptr) ? advisor->IsBlank(next) : -1;

		if (isAnnouncedBlank == 1)
		{
			bgraImage->AnnounceBlank();
		}
		else
		{
			const int tSize = PixelSize * this->frameSize.Area();
			int tRead = input->Read((unsigned __int8*)(image->GetDataBuffer()), 0, tSize);

			image->AnnounceModified();
			image->AnnounceNonstrictErased();

			if (tRead == tSize)
			{
				if (isAnnouncedBlank == 0)
				{
					bgraImage->AnnounceDirty();
				}
			}
			else
			{
				eos = true;
				bgraImage->AnnounceBlank();
				return -1;
			}
		}

		return (this->index = next);
	}

	BgraRawStream::~BgraRawStream()
	{
	}

	int BgraRawStream::SkipFrame(StillImage *image)
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

		return (this->index = next);
	}

	void BgraRawStream::SkipFrames(unsigned __int8 * buffer)
	{
		const int tSize = PixelSize * this->frameSize.Area();

		while (this->readIndex < this->index)
		{
			int tRead = input->Read(buffer, 0, tSize);
			if (tRead < tSize)
			{
				eos = true;
				return;
			}

			this->readIndex++;
		}
	}
}