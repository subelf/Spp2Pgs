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

#include "pch.h"

#include "GxBufferManager.h"

namespace spp2pgs
{
	int CompositionBuffer::EstimateDecodeDuration(int initDuration) const
	{
		if (windows == nullptr)
		{
			return 0;
		}
		
		int duration = initDuration;	//video init duration or window info decode duration.

		int minDurationBeforeWindow[WindowCountMax];	//decode duration of a window's contents
		int wndAccessOrderList[WindowCountMax + CompositionCountMax];
		int wndPosInOrderList[WindowCountMax];

		int decodeDuration = 0;
		int orderIndex = windows->count;

		memset(minDurationBeforeWindow, 0, sizeof(minDurationBeforeWindow));
		for (int i = 0; i < windows->count; ++i)
		{
			wndAccessOrderList[i] = wndPosInOrderList[i] = i;
		}

		for (int i = 0; i < cmpnObjsCount; ++i)
		{
			if (cmpnObjs[i].objectRef != nullptr //this object already exists, skip decoding
				&& (cmpnObjs[i].encodedObject == nullptr || cmpnObjs[i].encodedObjectLength == 0))
			{
				continue;
			}

			int tDecDur = cmpnObjs[i].EstimateDecodeDuration();
			
			int const &tWndRef = cmpnObjs[i].windowRef;

			tDecDur = (cmpnObjs[i].objectRef == nullptr)	//Not encoded yet.
				? min(tDecDur, GetObjectDecodeDuration(windows->windows[tWndRef].Area(), false))	//Estimate
				: tDecDur;	//Actual

			decodeDuration += tDecDur;
			minDurationBeforeWindow[tWndRef] = decodeDuration;

			int &tPos = wndPosInOrderList[tWndRef];
			if (tPos != -1) wndAccessOrderList[tPos] = -1;	//remove the previous record
			tPos = orderIndex;
			wndAccessOrderList[orderIndex++] = tWndRef;	//record the order windows are used.
		}

		for (int i = 0; i < orderIndex; ++i)
		{
			int const &j = wndAccessOrderList[i];	//sorted list

			if (j != -1)	//valid record
			{
				assert(j >= 0 && j < WindowCountMax);
				duration = max(duration, minDurationBeforeWindow[j]);
				duration += windows->EstimateDecodeDuration(j);
			}
		}

		return duration;
	}


	GxBufferManager::GxBufferManager(S2PContext const *context) :
		S2PControllerBase(context), 
		encodeBufferAllocPointer(0), maxDataSize(Size{ INT_MAX, INT_MAX })
	{
		for (int i = 0; i < CompositionCountMax; ++i)
		{
			paletteBuffers[i].GxBuffer::GxBuffer(i);
		}
		for (int i = 0; i < ObjectCountMax; ++i)
		{
			objectBuffers[i].GxBuffer::GxBuffer(i);
		}

		Clear();
	}

	GxBufferManager::~GxBufferManager()
	{
	}

	unsigned __int8 * GxBufferManager::AllocateEncodeBuffer(int size)
	{
		if (encodeBufferAllocPointer + size > EncodeBufferSize)
		{
			throw GxBufferException(GxBufferExceptionType::ResourceExhausted);
		}

		auto tRet = encodeBuffer + encodeBufferAllocPointer;
		encodeBufferAllocPointer += size;
		return tRet;
	}

	GxBuffer<PaletteBuffer> * GxBufferManager::FindFreePaletteBuffer()
	{
		int minVersion = 255;
		GxBuffer<PaletteBuffer> * pPltBuf = nullptr;

		for (int i = 0; i < CompositionCountMax; ++i)
		{
			auto &iPltBuf = paletteBuffers[i];
			
			if (iPltBuf.IsInUse())
			{
				continue;
			}

			if (iPltBuf.GetVersion() < minVersion)
			{
				minVersion = iPltBuf.GetVersion();
				pPltBuf = &iPltBuf;

				if (this->Settings()->IsForcingTmtCompat())
				{
					break;	//avoid using new palettes
				}
			}
		}

		if (pPltBuf == nullptr)
		{
			throw GxBufferException(GxBufferExceptionType::ResourceExhausted);
		}

		return pPltBuf;
	}

	GxBuffer<ObjectBuffer> * GxBufferManager::FindProperObjectBuffer(Size min, Size max)
	{
		Size limit = Size::MaxContents(max, this->maxDataSize);
		assert(min <= limit);

		Size tMin = (limit / 4);
		tMin = (min < tMin) ? tMin : min;
		Size half = tMin / 2;

		Size minSize{ INT_MAX, INT_MAX };
		GxBuffer<ObjectBuffer> * pObjBuf = nullptr;
		bool isSizeExist = false;	//is there a buffer just a little smaller than this image?

		for (int i = 0; i < ObjectCountMax; ++i)
		{
			auto &iObjBuf = objectBuffers[i];

			if (iObjBuf.IsUsed() && (iObjBuf->dataSize > half) && !(iObjBuf->dataSize > tMin))
			{
				isSizeExist = true;
			}

			if (iObjBuf.IsInUse())
			{
				continue;
			}
			
			if (iObjBuf.GetVersion() >= 255)
			{
				continue;
			}

			if (pObjBuf == nullptr && !iObjBuf.IsUsed())
			{
				pObjBuf = &iObjBuf;
				continue;
			}

			auto const &bufSize = iObjBuf->dataSize;
			if (bufSize >= min && bufSize <= limit && bufSize < minSize)
			{
				minSize = bufSize;
				pObjBuf = &iObjBuf;

				if (this->Settings()->IsForcingTmtCompat())
				{
					break;	//avoid using new image object buffer
				}
			}
		}

		if (pObjBuf == nullptr)
		{
			throw GxBufferException(GxBufferExceptionType::ResourceExhausted);
		}

		if (!pObjBuf->IsUsed())	//new obj buffer, suggest a size
		{
			int const &leftBuffer = ObjectBufferSizeMax - objectBufferSize;	//estimate free memory
			int tArea;

			if (pObjBuf->GetId() < 4 && (tArea = limit.Area()) < leftBuffer)	//first four obj buffer
			{
				(*pObjBuf)->dataSize = limit;	//prefer a big size
			}
			else
			{
				Size const &packedDoubleSize = limit.PackUp(tMin * 2);	//a 4-times-area-size
				if (isSizeExist && (tArea = packedDoubleSize.Area()) < leftBuffer)	//proper buffer exists but in use, and enough memory
				{
					(*pObjBuf)->dataSize = packedDoubleSize;	//create a larger one for this time and future use
				}
				else if ((tArea = tMin.Area()) < leftBuffer)
				{
					(*pObjBuf)->dataSize = tMin;	//use a smallest size in usual
				}
				else if ((tArea = min.Area()) < leftBuffer)
				{
					(*pObjBuf)->dataSize = min;	//memory is being used out, use a size equals to the image crop
				}
				else
				{
					throw GxBufferException(GxBufferExceptionType::ResourceExhausted);
				}
			}

			if (! this->Settings()->IsForcingTmtCompat())
			{
				objectBufferSize += tArea;	//forget everything about the buffer limit
			}
		}

		return pObjBuf;
	}

	GxBuffer<CompositionBuffer> * GxBufferManager::FindFreeCompositionBuffer()
	{
		for (int i = 0; i < CompositionCountMax; ++i)
		{
			auto &iPltBuf = compositionBuffers[i];
			if (!iPltBuf.IsInUse())
			{
				return &iPltBuf;
			}
		}

		throw GxBufferException(GxBufferExceptionType::ResourceExhausted);
	}

	GxBuffer<PaletteBuffer> * GxBufferManager::FindMatchedPaletteBuffer(int size, HashCode hash)
	{
		return nullptr;
	}

	GxBuffer<ObjectBuffer> * GxBufferManager::FindMatchedObjectBuffer(Size size, HashCode hash)
	{
		return nullptr;
	}

	void GxBufferManager::UpdateTime(__int64 dts)
	{
		for (int i = 0; i < CompositionCountMax; ++i)
		{
			GxBuffer<CompositionBuffer> &iBuf = compositionBuffers[i];

			if (!iBuf.IsInUse() || iBuf->pts > dts)	//still in use at this time
			{
				continue;
			}
			
			ReleaseCompositionBuffer(iBuf);

			iBuf.Release();
		}
	}

	void GxBufferManager::ReleaseCompositionBuffer(CompositionBuffer * buffer)
	{
		//release palette buffer
		GxBuffer<PaletteBuffer> const * iPltBuf = buffer->paletteRef;
		if (iPltBuf != nullptr)
		{
			paletteBuffers[iPltBuf->GetId()].Release();
			buffer->paletteRef = nullptr;
			buffer->encodedPalette = nullptr;
		}

		//release objects buffer
		for (int j = 0; j < buffer->cmpnObjsCount; ++j)
		{
			GxBuffer<ObjectBuffer> const * iObjBuf = buffer->cmpnObjs[j].objectRef;
			if (iObjBuf != nullptr)
			{
				objectBuffers[iObjBuf->GetId()].Release();
				buffer->cmpnObjs[j].objectRef = nullptr;
				buffer->cmpnObjs[j].encodedObject = nullptr;
			}
		}

		buffer->cmpnObjsCount = 0;
	}

	void GxBufferManager::Clear()
	{
		for (int i = 0; i < CompositionCountMax; ++i)
		{
			paletteBuffers[i].Clear();
			compositionBuffers[i].Clear();
		}
		for (int i = 0; i < ObjectCountMax; ++i)
		{
			objectBuffers[i].Clear();
		}

		objectBufferSize = 0;
	}
}