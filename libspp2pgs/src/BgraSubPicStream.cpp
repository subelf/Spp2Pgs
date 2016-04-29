/*----------------------------------------------------------------------------
* spp2pgs - Generates BluRay PG Stream from RGBA AviSynth scripts
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
#include "BgraSubPicStream.h"

namespace spp2pgs
{
	BgraSubPicStream::BgraSubPicStream(ISubPicProvider *spp, FrameStreamAdvisor const *advisor):
		FrameStream(), spp(spp), advisor(AssertArgumentNotNull(advisor)),
		frameCount(advisor->GetFirstPossibleImage()),
		index(advisor->GetLastPossibleImage()),
		frameSize(spp2pgs::GetFrameSize(advisor->GetFrameFormat())),
		frameRate(advisor->GetFrameRate())
	{
	}

	int BgraSubPicStream::GetNextFrame(StillImage * image)
	{
		int next = this->index + 1;

		if (next >= this->frameCount)
		{
			this->index = this->frameCount;
			image->AnnounceBlank();
			return -1;
		}

		int const &isAnnouncedBlank = (advisor != nullptr) ? advisor->IsBlank(next) : -1;

		if (isAnnouncedBlank == 1)
		{
			image->AnnounceBlank();
		}
		else
		{
			SubPicDesc spd = this->DescribeSubPicBuffer(image);
			auto const &rt = this->CalculateReferenceTime(next);
			double const &fps = spp2pgs::GetFrameRate(this->frameRate);
			RECT dirtyRegion;

			HRESULT hr = spp->Render(spd, rt, fps, dirtyRegion);

			if (hr == S_OK)
			{
				image->AnnounceModified();
				image->AnnounceNonstrictErased();
				if (isAnnouncedBlank == 0)
				{
					image->AnnounceDirty();
				}
			}
			else
			{
				image->AnnounceBlank();
				return -1;
			}
		}

		return (this->index = next);
	}

	inline SubPicDesc BgraSubPicStream::DescribeSubPicBuffer(StillImage * image)
	{
		bool const &isValid = (image->GetPixelSize() == BgraFrame::PixelSize) &&
			(image->GetWidth() == this->frameSize.w) &&
			(image->GetHeight() == this->frameSize.h);
		if (!isValid)
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImagePixelFormat);
		}
		
		SubPicDesc spd;
		spd.type = MSP_RGBA;
		spd.bits = image->GetDataBuffer();
		spd.bpp = image->GetPixelSize() << 3;
		spd.w = image->GetWidth();
		spd.h = image->GetHeight();
		spd.pitch = image->GetStride();

		return spd;
	}

	inline REFERENCE_TIME BgraSubPicStream::CalculateReferenceTime(int frameIndex)
	{
		int const tps = 10000000;	//100ns per tick;

		auto const& frameRateInfo = FrameRateTable[(int)this->frameRate];
		long long const& frames = frameIndex;
		int const& FramesPerUnit = frameRateInfo.framesPerUnit;
		int const& TicksPerSecond = tps;
		int const& SecondsPerUnit = frameRateInfo.secondsPerUnit;

		return frames * TicksPerSecond * SecondsPerUnit / FramesPerUnit;
	}

}