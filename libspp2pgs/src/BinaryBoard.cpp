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
#include "BinaryBoard.h"

namespace avs2pgs
{
	static bool IsCropBlank(BgraFrame *src, Rect crop)
	{
		char *srcImg = src->GetDataBuffer();
		int stride = src->GetStride();
		
		Border b = crop.ToOuterBorder();

		for (int y = crop.y; y < b.bottom; ++y)
		{
			Pixel32 *line = (Pixel32*) (srcImg + y * stride);
			for (int x = crop.x; x < b.right; ++x)
			{
				if (line[x].value) return false;
			}
		}

		return true;
	}

	BinaryBoard::BinaryBoard(Size size, int maxBlockSize)
		: buffer(CreateBuffer(size, maxBlockSize))
	{
		Clear();
	}

	BinaryBoard::BinaryBoard(BgraFrame *src, int maxBlockSize)
		: buffer(CreateBuffer(src->GetImageSize(), maxBlockSize))
	{
		Fill(src);
	}

	BinaryBoard::~BinaryBoard()
	{
		if (this->buffer != nullptr)
		{
			delete[] this->buffer;
			this->buffer = nullptr;
		}
	}

	void BinaryBoard::Clear()
	{
		memset(buffer, 0, bufferSize);
		cropCache = Rect{ MAXINT, MAXINT, MININT, MININT };
	}

	void BinaryBoard::Fill(BgraFrame *src)
	{
		AssertCompatibleTo(src);

		Clear();

		if (src->IsBlank())
		{
			return;
		}

		Border b = { MAXINT, MAXINT, MININT, MININT };

		char const Mask0 = (char)0x01, Mask8 = (char)0x80;
		for (int y = 0; y < boardSize.h; ++y)
		{
			char* line = buffer + stride * y;
			char mask = Mask0;
			for (int x = 0; x < boardSize.w; ++x, mask = (mask == Mask8) ? Mask0 : (mask << 1))
			{
				Rect blockCrop { x * blockSize, y * blockSize, blockSize, blockSize };
				bool isCropBlank = IsCropBlank(src, blockCrop);
				line[x >> BitPerByteIndex] |= (isCropBlank ? 0 : mask);
				if (!isCropBlank)
				{
					b.left = (b.left > x) ? x : b.left;
					b.right = (b.right < x) ? x : b.right;
					b.top = (b.top > y) ? y : b.top;
					b.bottom = (b.bottom < y) ? y : b.bottom;
				}
			}
		}

		if (b.right >= 0)
		{
			cropCache = Rect::FromOuterBorder(b.ToOuterBorder()) * blockSize;
		}

		return;
	}

	Rect BinaryBoard::QuickCrop(Rect window) const
	{
		AssertCompatibleTo(&window);

		Rect tRet = { 0, 0, 0, 0 };
		Rect tBbWnd = (window * cropCache) / blockSize;

		Border b{ MAXINT, MAXINT, MININT, MININT };
		Border wndBorder = tBbWnd.ToOuterBorder();
		
		for (int y = wndBorder.top; y < wndBorder.bottom; ++y)
		{
			char* line = buffer + stride * y;
			for (int x = wndBorder.left; x < wndBorder.right; ++x)
			{
				char mask = 1 << (x & ByteMask);
				if (line[x >> BitPerByteIndex] & mask)
				{
					b.left = (b.left > x) ? x : b.left;
					b.right = (b.right < x) ? x : b.right;
					b.top = (b.top > y) ? y : b.top;
					b.bottom = (b.bottom < y) ? y : b.bottom;
				}
			}
		}

		if (b.right < 0)
		{
			return tRet;
		}

		tRet = Rect::FromOuterBorder(b.ToOuterBorder()) * blockSize;

		return tRet * window;
	}


	int BinaryBoard::QuickSplit(Rect originRect, Rect output[2], bool horizonFirst) const
	{
		int tRet = 0;
		if (horizonFirst)
		{
			tRet = QuickSplitHorizontally(originRect, output);
		}

		if (tRet <= 1)
		{
			tRet = QuickSplitVertically(originRect, output);
		}
		
		if (tRet <= 1 && !horizonFirst)
		{
			tRet = QuickSplitHorizontally(originRect, output);
		}

		return tRet;
	}

	int BinaryBoard::QuickSplitHorizontally(Rect originRect, Rect output[2]) const
	{
		Rect tOrigin = QuickCrop(originRect);

		Border b = (tOrigin / blockSize).ToOuterBorder();
		int splitAt = -1;

		for (int x = b.right - 2; x > b.left; --x)
		{
			bool blank = true;
			for (int y = b.top; y < b.bottom; ++y)
			{
				if (!IsBlank(x, y))
				{
					blank = false;
					break;
				}
			}

			if (blank)
			{
				splitAt = x * blockSize;
				break;
			}
		}

		if (splitAt == -1)
		{
			output[0] = tOrigin;
			return 1;
		}

		b = tOrigin.ToOuterBorder();
		b.right = splitAt;
		output[0] = QuickCrop(Rect::FromOuterBorder(b));

		b = tOrigin.ToOuterBorder();
		b.left = splitAt;
		output[1] = QuickCrop(Rect::FromOuterBorder(b));

		return 2;
	}

	int BinaryBoard::QuickSplitVertically(Rect originRect, Rect output[2]) const
	{
		Rect tOrigin = QuickCrop(originRect);

		Border b = (tOrigin / blockSize).ToOuterBorder();
		int splitAt = -1;

		for (int y = b.bottom - 2; y > b.top; --y)
		{
			bool blank = true;
			for (int x = b.left; x < b.right; ++x)
			{
				if (!IsBlank(x, y))
				{
					blank = false;
					break;
				}
			}

			if (blank)
			{
				splitAt = y * blockSize;
				break;
			}
		}

		if (splitAt == -1)
		{
			output[0] = tOrigin;
			return 1;
		}

		b = tOrigin.ToOuterBorder();
		b.top = splitAt;
		output[0] = QuickCrop(Rect::FromOuterBorder(b));

		b = tOrigin.ToOuterBorder();
		b.bottom = splitAt;
		output[1] = QuickCrop(Rect::FromOuterBorder(b));

		return 2;
	}

	Rect BinaryBoard::Crop(Rect window, IndexedImage const * src) const
	{
		AssertCompatibleTo(src);

		Rect tRet = { 0, 0, 0, 0 };
		if (src->IsBlank())
		{
			return tRet;
		}

		Rect iRect = QuickCrop(window);

		if (iRect.w == 0)
		{
			return tRet;
		}
		
		char* srcImg = src->GetDataBuffer();
		int const iStride = src->GetStride();

		Border iB = iRect.ToOuterBorder();

		int tmp = iB.top + blockSize;
		for (int y = iB.top; tmp > 0 && y < tmp; ++y)
		{
			char* line = (srcImg + y * iStride);
			for (int x = iB.left; x < iB.right; ++x)
			{
				if (line[x])
				{
					tmp = -1;
					iB.top = y;
					break;
				}
			}
		}

		tmp = iB.bottom - blockSize;
		for (int y = iB.bottom - 1; tmp > 0 && y >= tmp; --y)
		{
			char* line = (srcImg + y * iStride);
			for (int x = iB.left; x < iB.right; ++x)
			{
				if (line[x])
				{
					tmp = -1;
					iB.bottom = y + 1;
					break;
				}
			}
		}

		tmp = iB.left + blockSize;
		for (int x = iB.left; tmp > 0 && x < tmp; ++x)
		{
			for (int y = iB.top; y < iB.bottom; ++y)
			{
				if ((srcImg + y * iStride)[x])
				{
					tmp = -1;
					iB.left = x;
					break;
				}
			}
		}

		tmp = iB.right - blockSize;
		for (int x = iB.right - 1; tmp > 0 && x > tmp; --x)
		{
			for (int y = iB.top; y < iB.bottom; ++y)
			{
				if ((srcImg + y * iStride)[x])
				{
					tmp = -1;
					iB.right = x + 1;
					break;
				}
			}
		}

		return Rect::FromOuterBorder(iB);
	}


	void BinaryBoard::MergeFrom(const BinaryBoard *board)
	{
		AssertCompatibleTo(board);

		for (int y = 0; y < boardSize.h; ++y)
		{
			auto tLineOfs = stride * y;
			char* line = buffer + tLineOfs;
			char* srcLine = board->buffer + tLineOfs;

			for (int x = 0; x < stride; ++x)
			{
				line[x] |= srcLine[x];
			}
		}

		cropCache = cropCache + board->cropCache;
	}

	inline void BinaryBoard::AssertCompatibleTo(StillImage const *src) const
	{
		if (src == nullptr)
		{
			throw A2PException(A2PExceptionType::ArgumentNull, nullptr);
		}
		
		if (src->GetImageSize() != boardSize * blockSize)
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImageSize);
		}
	}

	inline void BinaryBoard::AssertCompatibleTo(BinaryBoard const *board) const
	{
		if (board == nullptr)
		{
			throw A2PException(A2PExceptionType::NotImplenmented, nullptr);
		}

		if ((board->boardSize != boardSize))
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImageSize);
		}
	}

	inline void BinaryBoard::AssertCompatibleTo(Rect const *rect) const
	{
		if (rect == nullptr)
		{
			throw A2PException(A2PExceptionType::NotImplenmented, nullptr);
		}

		Rect canvasRect { 0, 0, boardSize * blockSize };

		if (!canvasRect.IsContains(*rect))
		{
			throw ImageOperationException(ImageOperationExceptionType::InvalidImageSize);
		}
	}

	inline char* BinaryBoard::CreateBuffer(Size size, int maxBlockSize)
	{
		maxBlockSize = max(maxBlockSize, 0);

		int tLimit = maxBlockSize ? maxBlockSize : (size.w >> 6);
		
		auto tGcd = GcdOf(size.w, size.h);

		blockSize = (int) GdBelow(tGcd, tLimit);
		boardSize = size / blockSize;

		stride = (boardSize.w + ByteMask) >> BitPerByteIndex;

		return new char[bufferSize = stride * boardSize.h];
	}
}