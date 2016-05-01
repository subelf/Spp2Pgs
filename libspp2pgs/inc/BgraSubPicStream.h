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

#pragma once

#include <VSSppfApi.h>

#include "GraphicalTypes.h"
#include "S2PExceptions.h"
#include "FrameStream.h"

namespace spp2pgs
{

	class BgraSubPicStream :
		public FrameStream
	{
	public:
		BgraSubPicStream(ISubPicProviderAlfa *spp, FrameStreamAdvisor const *advisor) throw(S2PException);

		int GetNextFrame(StillImage *image);

		inline int GetFrameCount() const { return this->frameCount; }
		inline int GetCurrentIndex() const { return this->index; }
		inline Size GetFrameSize() const { return this->frameSize; }
		inline BdViFrameRate GetFrameRate() const { return this->frameRate; }
		inline FrameStreamAdvisor const * GetAdvisor() const { return this->advisor; }

	private:
		int index;
		int frameCount;
		Size frameSize;
		BdViFrameRate frameRate;

		CComPtr<ISubPicProviderAlfa> spp;
		FrameStreamAdvisor const *advisor;
	};

}