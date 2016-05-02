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

/*----------------------------------------------------------------------------
* Imported from muxers.c of the x264 project (GPLv2 or later).
* Authors: Laurent Aimar <fenrir@via.ecp.fr>
*          Loren Merritt <lorenm@u.washington.edu>
* and last modified by Giton<adm@subelf.net>
*----------------------------------------------------------------------------*/

#include "pch.h"
#include "BgraFrame.h"
#include "BgraAvsStream.h"

namespace spp2pgs
{

	BgraAvsStream::BgraAvsStream(TCHAR* avspath, FrameStreamAdvisor const * advisor) :
		FrameStream(), advisor(AssertArgumentNotNull(advisor)), index(-1),
		indexOffset(advisor->GetFrameIndexOffset()), frameCount(-1), 
		frameSize({ 0, 0 }), frameRate(BdViFrameRate::Unknown)
	{
		this->OpenAvsFile(avspath);
	}

	void BgraAvsStream::OpenAvsFile(TCHAR* avspath)
	{
		AVISTREAMINFO info;
		HRESULT result;

		AVIFileInit();
		if (result = AVIStreamOpenFromFile(&this->stream, avspath, streamtypeVIDEO, 0, OF_READ, nullptr))
		{
			AVIFileExit();
			throw AvsInitException(AvsInitExceptionType::AvsStreamOpenFailed, result);
		}

		if (result = AVIStreamInfo(this->stream, &info, sizeof(AVISTREAMINFO)))
		{
			this->CloseAvsFile();
			throw AvsInitException(AvsInitExceptionType::AvsGetInfoFailed, result);
		}

		/* Check input format */
		if (result = info.fccHandler != MAKEFOURCC('D', 'I', 'B', ' '))
		{
			this->CloseAvsFile();
			throw AvsInitException(AvsInitExceptionType::AvsFormatIncorrect, result);
		}

		if (advisor != nullptr)
		{
			this->frameSize = spp2pgs::GetFrameSize(advisor->GetFrameFormat());
			this->frameRate = advisor->GetFrameRate();
			this->frameCount = advisor->GetLastPossibleImage() - indexOffset;
			this->index = advisor->GetFirstPossibleImage() - indexOffset - 1;
		}

		this->frameSize = (this->frameSize.Area() == 0) ? Size{ info.rcFrame.right - info.rcFrame.left, info.rcFrame.bottom - info.rcFrame.top } : this->frameSize;
		this->frameRate = (this->frameRate == BdViFrameRate::Unknown) ? spp2pgs::GuessBdFrameRateFrom(info.dwRate / (double)info.dwScale) : this->frameRate;
		this->frameCount = (this->frameCount == -1) ? info.dwLength : this->frameCount;
		this->index = (this->index < 0) ? -1 : this->index;

		if (this->frameSize.Area() == 0 || this->frameRate == BdViFrameRate::Unknown)
		{
			throw S2PException(S2PExceptionType::Unknown, nullptr);
		}

		return;
	}

	void BgraAvsStream::CloseAvsFile()
	{
		if (this->stream != nullptr)
		{
			AVIStreamRelease(this->stream);
			AVIFileExit();
			this->stream = nullptr;
		}
	}

	int BgraAvsStream::GetNextFrame(StillImage *image)
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

		int const &isAnnouncedBlank = (advisor != nullptr) ? advisor->IsBlank(next + indexOffset) : -1;

		if (isAnnouncedBlank == 1)
		{
			bgraImage->AnnounceBlank();
		}
		else
		{
			HRESULT hr = AVIStreamRead(this->stream, next, 1, bgraImage->GetDataBuffer(), bgraImage->GetDataSize(), NULL, NULL);

			if (hr == S_OK)
			{
				bgraImage->AnnounceModified();
				bgraImage->AnnounceNonNormalized();
				if (isAnnouncedBlank == 0)
				{
					bgraImage->AnnounceDirty();
				}
			}
			else
			{
				bgraImage->AnnounceBlank();
				return -1;
			}
		}

		return (this->index = next) + indexOffset;
	}

	int BgraAvsStream::SkipFrame(StillImage *image)
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

		return (this->index = next) + indexOffset;
	}

	BgraAvsStream::~BgraAvsStream()
	{
		this->CloseAvsFile();
	}
}