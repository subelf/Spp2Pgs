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

#pragma once

#include <queue>

#include "S2PControllerBase.h"
#include "EpochCache.h"
#include "DisplaySetThumb.h"
#include "BlurayCommon.h"
#include "IndexedImage.h"

namespace spp2pgs
{

	class EpochManager final
		: public S2PControllerBase
	{
	public:
		EpochManager(S2PContext const * context, BdViFrameRate frameRate, Size frameSize) throw(TempOutputException);
		~EpochManager() {};

		bool IsNewEpochStart(DisplaySetThumb *thumb);
		void EnqueueDisplaySet(DisplaySetThumb *thumb, IndexedImage *imageBuffer) throw(TempOutputException, ImageOperationException);
		DisplaySetThumb *DequeueDisplaySet(IndexedImage *imageBuffer) throw(TempOutputException, ImageOperationException);
		inline BinaryBoard const * GetMergedBinaryBoard() { return &binBoard; }
		inline Size const & GetMaxOfCropSize() { return cropSizeMax; }
		inline bool IsEmpty() { return cmpnQueue.empty(); }
		inline int Length() { return static_cast<int>(cmpnQueue.size()); }

	private:
		Size frameSize;
		int minInterval;

		std::queue<DisplaySetThumb*> cmpnQueue;
		BinaryBoard binBoard;
		Size cropSizeMax;
		EpochCache cache;

		bool isDequeuing;
	};


}