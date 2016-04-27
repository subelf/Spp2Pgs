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

#pragma once

#include "BlurayCommon.h"
#include "FrameStreamAdvisor.h"
#include "StillImage.h"

namespace avs2pgs
{

	class FrameStream
	{
	protected:
		FrameStream() {};

	public:
		virtual int GetNextFrame(StillImage *image) = 0;

		virtual int GetFrameCount() const = 0;
		virtual int GetCurrentIndex() const = 0;
		virtual Size GetFrameSize() const = 0;
		virtual BdViFrameRate GetFrameRate() const = 0;
		virtual FrameStreamAdvisor const * GetAdvisor() const = 0;

		virtual ~FrameStream() {};
	};

}