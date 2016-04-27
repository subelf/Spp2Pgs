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

#include "StillImage.h"
#include "FrameStreamAdvisor.h"
#include "GraphicalTypes.h"
#include "FrameStream.h"

namespace spp2pgs
{

	class BgraFrame
		:public StillImage
	{
	public:
		static int const PixelSize = 4;

		BgraFrame(Size frameSize);

		inline int ReadNextOf(FrameStream * stream) {
			this->index = stream->GetNextFrame(this);
			this->advisor = stream->GetAdvisor();
			return this->index;
		}

		inline int GetFrameIndex() { return this->index; }

		bool IsIdenticalTo(BgraFrame *frame);
		bool IsBlank();

		~BgraFrame();

	protected:
		bool IsExplicitIdenticalTo(StillImage const *image) const override;
		bool IsExplicitBlank() const override;
		void ExplicitEraseTransparents() const override;
		void ExplicitErase(Rect rect) const override;

	private:
		FrameStreamAdvisor const * advisor;
		int index;
	};

}