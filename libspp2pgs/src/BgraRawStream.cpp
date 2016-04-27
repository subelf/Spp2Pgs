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


#include "pch.h"
#include "BgraRawStream.h"

namespace avs2pgs
{

	BgraRawStream::BgraRawStream(StreamEx * stream, FrameStreamAdvisor const * advisor)
		:input(stream), advisor(advisor), index(-1), readIndex(-1), eos(false)
	{
		if (stream == nullptr || advisor == nullptr)
		{
			throw A2PException(A2PExceptionType::ArgumentNull, nullptr);
		}

		this->frameSize = avs2pgs::GetFrameSize(advisor->GetFrameFormat());
		this->frameRate = advisor->GetFrameRate();
		this->frameCount = advisor->GetLastPossibleImage();
		this->index = advisor->GetFirstPossibleImage() - 1;

		if (this->frameSize.Area() == 0 || this->frameRate == BdViFrameRate::Unknown)
		{
			throw A2PException(A2PExceptionType::Unknown, nullptr);
		}

		this->index = (this->index < 0) ? -1 : this->index;
	}

	int BgraRawStream::GetNextFrame(StillImage *image)
	{
		int next = this->index + 1;
		SkipFrames((unsigned __int8*)(image->GetDataBuffer()));

		if (eos || (this->frameCount != -1 && next >= this->frameCount))
		{
			eos = true;
			image->AnnounceBlank();
			return -1;
		}

		const int tSize = PixelSize * this->frameSize.Area();
		int tRead = input->Read((unsigned __int8*)(image->GetDataBuffer()), 0, tSize);
		image->AnnounceModified();
		image->AnnounceNonstrictErased();

		if (tRead < tSize)
		{
			eos = true;
			return -1;
		}

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

		return (this->index = next);
	}

	BgraRawStream::~BgraRawStream()
	{
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