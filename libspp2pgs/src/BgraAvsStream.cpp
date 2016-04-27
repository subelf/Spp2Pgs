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

/*----------------------------------------------------------------------------
* Imported from muxers.c of the x264 project (GPLv2 or later).
* Authors: Laurent Aimar <fenrir@via.ecp.fr>
*          Loren Merritt <lorenm@u.washington.edu>
* and last modified by Giton<adm@subelf.net>
*----------------------------------------------------------------------------*/

#include "pch.h"
#include "BgraAvsStream.h"

namespace avs2pgs
{

	BgraAvsStream::BgraAvsStream(TCHAR* avspath, FrameStreamAdvisor const * advisor)
		:FrameStream(), index(-1), advisor(advisor), frameSize({ 0, 0 }), frameRate(BdViFrameRate::Unknown), frameCount(-1)
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
			this->frameSize = avs2pgs::GetFrameSize(advisor->GetFrameFormat());
			this->frameRate = advisor->GetFrameRate();
			this->frameCount = advisor->GetLastPossibleImage();
			this->index = advisor->GetFirstPossibleImage() - 1;
		}
		
		this->frameSize = (this->frameSize.Area() == 0) ? Size{ info.rcFrame.right - info.rcFrame.left, info.rcFrame.bottom - info.rcFrame.top } : this->frameSize;
		this->frameRate = (this->frameRate == BdViFrameRate::Unknown) ? avs2pgs::GuessBdFrameRateFrom(info.dwRate / (double)info.dwScale) : this->frameRate;
		this->frameCount = (this->frameCount == -1) ? info.dwLength : this->frameCount;
		this->index = (this->index < 0) ? -1 : this->index;

		if (this->frameSize.Area() == 0 || this->frameRate == BdViFrameRate::Unknown)
		{
			throw A2PException(A2PExceptionType::Unknown, nullptr);
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
		int next = this->index + 1;

		if (next >= this->frameCount)
		{
			this->index = this->frameCount;
			image->AnnounceBlank();
			return -1;
		}

		HRESULT result = AVIStreamRead(this->stream, next, 1, image->GetDataBuffer(), image->GetDataSize(), NULL, NULL);
		image->AnnounceModified();
		image->AnnounceNonstrictErased();

		if (advisor != nullptr)
		{
			switch (advisor->IsBlank(next))
			{
			case 0:
				image->AnnounceDirty();
				break;
			case 1:
				image->AnnounceBlank();
				break;
			}
		}
		
		if (result)
		{
			return -1;
		}

		return (this->index = next);
	}

	BgraAvsStream::~BgraAvsStream()
	{
		this->CloseAvsFile();
	}
}