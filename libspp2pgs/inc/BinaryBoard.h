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

#include "BgraFrame.h"
#include "IndexedImage.h"
#include "GraphicalTypes.h"
#include "BinaryBoard.h"

namespace avs2pgs
{
	class BinaryBoard final
	{
	public:
		BinaryBoard(Size size, int maxBlockSize = 0);
		BinaryBoard(BgraFrame *src, int maxBlockSize = 0);
		~BinaryBoard();

		void Clear();
		void Fill(BgraFrame *src);
		void MergeFrom(const BinaryBoard *board);
		inline bool IsBlank(int x, int y) const 
		{
			return !(buffer[stride * y + (x >> BitPerByteIndex)] & (1 << (x & ByteMask)));
		}

		Rect Crop(Rect window, IndexedImage const *src) const;

		inline Rect QuickCrop() const { return this->cropCache; }
		Rect QuickCrop(Rect window) const;

		inline int QuickSplit(Rect output[2], bool horizonFirst = false) const
		{
			return QuickSplit(cropCache, output, horizonFirst);
		}
		int QuickSplit(Rect originRect, Rect output[2], bool horizonFirst = false) const;
		int QuickSplitHorizontally(Rect originRect, Rect output[2]) const;
		int QuickSplitVertically(Rect originRect, Rect output[2]) const;

		inline Size const GetBoardSize() const { return boardSize; }
		inline int const GetBlockSize() const { return blockSize; }

	private:
		int blockSize;
		Size boardSize;
		int stride;
		int bufferSize;
		char* buffer;

		Rect cropCache;

		char* CreateBuffer(Size size, int maxBlockSize);

		void AssertCompatibleTo(StillImage const *src) const;
		void AssertCompatibleTo(BinaryBoard const *board) const;
		void AssertCompatibleTo(Rect const *rect) const;

		static int const BitPerByteIndex = 3;
		static int const ByteMask = (1 << BitPerByteIndex) - 1;
	};

}