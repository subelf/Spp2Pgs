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

#include "CompositionEncoder.h"

namespace spp2pgs
{

	CompositionEncoder::CompositionEncoder(S2PContext const *context, WindowsDescriptor const * windows, GxBufferManager * bufMgr) :
		S2PControllerBase(context),
		CompositionBuffer(CompositionBuffer{ 0,0 , windows, nullptr, 0, nullptr, 0 }),
		bufMgr(bufMgr)
	{
	}

	static auto CropAreaCompare(void const *p1, void const* p2) {
		auto const &crop1 = reinterpret_cast<CompositionObjectDescriptor const *>(p1);
		auto const &crop2 = reinterpret_cast<CompositionObjectDescriptor const *>(p2);
		return crop2->imageCrop.Area() - crop1->imageCrop.Area();	//decendant
	}

	CompositionBuffer const * CompositionEncoder::Encode(DisplaySetThumb const * thumb, IndexedImage const * image)
	{
		this->displaySet = thumb;
		this->image = image;

		this->pts = thumb->GetPTS();
		this->ets = thumb->GetETS();

		try
		{
			if (this->Settings()->IsForcingTmtCompat())
			{
				this->SplitWithoutCropping();	//keep stable image in presenting windows
				this->bufMgr->UpdateTime(this->pts - 1);	//ignore decode duration
			}
			else
			{
				this->Split();
			}

			qsort(cmpnObjs, cmpnObjsCount, sizeof(CompositionObjectDescriptor),	&CropAreaCompare);	//minimize decode duration

			__int64 dtsEst = this->pts - this->EstimateDecodeDuration(1);
			this->bufMgr->UpdateTime(dtsEst);

			this->EncodeObjects();

			dtsEst = this->pts - this->EstimateDecodeDuration(1);
			this->bufMgr->UpdateTime(dtsEst);

			this->EncodePalette();

			auto* cmpn = this->bufMgr->FindFreeCompositionBuffer();
			**cmpn = *dynamic_cast<CompositionBuffer *>(this);
			cmpn->AddRef();

			return *cmpn;
		}
		catch (GxBufferException ex)
		{
			bufMgr->ReleaseCompositionBuffer(this);
			throw ex;
		}
	}

	void CompositionEncoder::Split()
	{
		auto crop = displaySet->GetDataCrop();
		auto binBoard = displaySet->GetBinaryBoard();

		int &iCrop = cmpnObjsCount = 0;
		
		for (int i = 0; i < windows->count; ++i)
		{
			if (crop.IsIntersectsWith(windows->windows[i]))	//window used
			{
				if (windows->windows[i].IsContains(crop))	//only window[i] is used.
				{
					Rect cmpnRects[CompositionObjectCountMax];
					iCrop = binBoard->QuickSplit(crop, cmpnRects);

					if (iCrop > 1)
					{
						int totalArea = 0;
						for (int j = 0; j < iCrop; ++j) totalArea += cmpnRects[j].Area();

						if (totalArea >(windows->windows[i].Area() >> 1))	//big image won't be cropped.
						{
							cmpnObjs[0].imageCrop = binBoard->Crop(windows->windows[i], image);
							cmpnObjs[0].windowRef = i;

							iCrop = 1;
							break;
						}
					}

					for (int j = 0; j < iCrop; ++j)
					{
						cmpnObjs[j].windowRef = i;
						cmpnObjs[j].imageCrop = binBoard->Crop(cmpnRects[j], image);
					}

					break;
				}
				else
				{
					cmpnObjs[iCrop].imageCrop = binBoard->Crop(windows->windows[i], image);
					cmpnObjs[iCrop].windowRef = i;
					iCrop++;
				}
			}
		}

		for (int i = 0; i < cmpnObjsCount; ++i)
		{
			cmpnObjs[i].objectRef = nullptr;
			cmpnObjs[i].encodedObject = nullptr;
		}
	}

	void CompositionEncoder::SplitWithoutCropping()
	{
		auto crop = displaySet->GetDataCrop();
		auto binBoard = displaySet->GetBinaryBoard();

		int &iCrop = cmpnObjsCount = 0;

		for (int i = 0; i < windows->count; ++i)
		{
			if (crop.IsIntersectsWith(windows->windows[i]))	//window used
			{
				if (windows->windows[i].IsContains(crop))	//only window[i] is used.
				{
					assert(iCrop == 0);

					cmpnObjs[0].imageCrop = windows->windows[i];
					cmpnObjs[0].windowRef = i;
					iCrop = 1;

					break;
				}
				else
				{
					cmpnObjs[iCrop].imageCrop = windows->windows[i];
					cmpnObjs[iCrop].windowRef = i;
					iCrop++;
				}
			}
		}

		for (int i = 0; i < cmpnObjsCount; ++i)
		{
			cmpnObjs[i].objectRef = nullptr;
			cmpnObjs[i].encodedObject = nullptr;
		}
	}
	
	void CompositionEncoder::EncodeObjects()
	{
		for(int i = 0; i < cmpnObjsCount; ++i)
		{
			HashCode objHash = GetImageHash(cmpnObjs[i].imageCrop);
			cmpnObjs[i].encodedObjectLength = 0;
			cmpnObjs[i].encodedObject = nullptr;

			auto objBuf = bufMgr->FindMatchedObjectBuffer(cmpnObjs[i].imageCrop.size, objHash);

			if (!IsImageBufferValid(i))
			{
				objBuf = nullptr;
			}

			if (objBuf == nullptr)
			{
				auto maxRect = GetMaxAvailableRect(i);
				objBuf = bufMgr->FindProperObjectBuffer(cmpnObjs[i].imageCrop.size, maxRect.size);
								
				Position tRectPos = ArrangeRect(cmpnObjs[i].imageCrop, maxRect, (*objBuf)->dataSize);	//get object's offset on screen
				
				(*objBuf)->hashRect = cmpnObjs[i].imageCrop - tRectPos;
				(*objBuf)->imageHash = objHash;

				cmpnObjs[i].objectPos = tRectPos;

				//if support cropping pgs { } else:
				cmpnObjs[i].imageCrop = Rect{ tRectPos, (*objBuf)->dataSize };

				cmpnObjs[i].encodedObject = bufMgr->AllocateEncodeBuffer((*objBuf)->dataSize.Area());
				cmpnObjs[i].encodedObjectLength = EncodeImageData(i, cmpnObjs[i].encodedObject, (*objBuf)->dataSize);

				objBuf->UpdateVersion();
			}

			cmpnObjs[i].objectRef = objBuf;

			objBuf->AddRef();
		}
	};

	inline unsigned __int8 * WriteRleData(char value, int len, unsigned __int8 * dst)
	{
		unsigned __int8 const WordValueMask = 0x40;
		unsigned __int8 const NonZeroMask = 0x80;

		if (!value || len >2)
		{
			*dst++ = 0;
			unsigned __int8 nzMask = value ? NonZeroMask : 0;

			if (len >= WordValueMask)
			{
				*dst++ = (nzMask | WordValueMask | (len >> 8));
				nzMask = 0;
			}

			*dst++ = (nzMask | len);
		}

		if (value)
		{
			if (len == 2)*dst++ = value;
			*dst++ = value;
		}

		return dst;
	}

	int CompositionEncoder::EncodeImageData(int oid, unsigned __int8 * buffer, Size objSize)
	{
		int const imageHeight = image->GetImageSize().h;

		auto const * const src = image->GetDataBuffer();	//indexed image;
		auto const stride = image->GetStride();
		
		auto * dst = buffer;	//indexed image;

		auto const bImage = cmpnObjs[oid].imageCrop.ToOuterBorder();
		auto const bCanvas = Rect{ cmpnObjs[oid].objectPos, objSize }.ToOuterBorder();

		for (int y = bCanvas.top; y < bCanvas.bottom; ++y)
		{
			auto const * const line = src + stride * y;
			int runLen;

			if (y >= bImage.top && y < bImage.bottom)
			{
				runLen = bImage.left - bCanvas.left;
				if (runLen > 0)
				{
					dst = WriteRleData(0, runLen, dst);
				}

				int x = bImage.left;

				while (x < bImage.right)
				{
					char runChar = line[x++];
					runLen = 1;

					while (x < bImage.right && runChar == line[x])
					{
						runLen++;
						x++;
					}

					dst = WriteRleData(runChar, runLen, dst);
				}

				runLen = bCanvas.right - bImage.right;
				if (runLen > 0)
				{
					dst = WriteRleData(0, runLen, dst);
				}
			}
			else
			{
				runLen = bCanvas.right - bCanvas.left;
				if (runLen > 0)
				{
					dst = WriteRleData(0, runLen, dst);
				}
			}
			
			*dst++ = 0;
			*dst++ = 0;
		}

		return static_cast<int>(dst - buffer);
	}

	void CompositionEncoder::EncodePalette() {

		HashCode palHash = GetPaletteHash();
		encodedPaletteLength = 0;
		encodedPalette = nullptr;

		auto palBuf = bufMgr->FindMatchedPaletteBuffer(image->GetPaletteLength(), palHash);

		if (palBuf == nullptr)
		{
			palBuf = bufMgr->FindFreePaletteBuffer();

			(*palBuf)->hashLength = image->GetPaletteLength();
			(*palBuf)->hash = palHash;
			
			encodedPalette = bufMgr->AllocateEncodeBuffer(EncodedPaletteItemSize * image->GetPaletteLength());
			this->encodedPaletteLength = EncodePaletteData(encodedPalette);

			palBuf->UpdateVersion();
		}

		paletteRef = palBuf;

		palBuf->AddRef();
	}

	int CompositionEncoder::EncodePaletteData(unsigned __int8 * buffer)
	{
		static int const Rgb2YuvFactor_15[3][3][4] = {
			{//Rgb --> BT.601, kr=0.114, kb=0.299
				{ 8414, 16519, 3208, 524288 },
				{ -4857, -9535, 14392, 4194304 },
				{ 14392, -12052, -2341, 4194304 },
			},
			{//Rgb --> BT.709, kr=0.0722, kb=0.2126
				{ 5983, 20127, 2032, 524288 },
				{ -3298, -11094, 14392, 4194304 },
				{ 14392, -13073, -1320, 4194304 },
			},
			{//Rgb --> BT.2020, kr=0.0593, kb=0.2627
				{ 7393, 19080, 1669, 524288 },
				{ -4019, -10373, 14392, 4194304 },
				{ 14392, -13235, -1158, 4194304 },
			},
		};

		int const imageHeight = image->GetImageSize().h;

		Pixel32 const * const src = image->GetPalette();	//bgra palette. 
		EncodedPalette * const dst = reinterpret_cast<EncodedPalette *>(buffer);

		int const paletteLength = image->GetPaletteLength();
		int const yuvMode = (imageHeight < 720) ? 0 : ((imageHeight > 1080) ? 2 : 1);

		for (int i = 0; i < paletteLength; ++i)
		{
			auto bgr = src[i].bgr;
			dst[i].i = i;
			dst[i].a = src[i].a;
			
			int const *t;
			t = Rgb2YuvFactor_15[yuvMode][0]; dst[i].y = (((bgr.r * t[0] + bgr.g * t[1] + bgr.b * t[2] + t[3]) >> 14) +1) >> 1;
			t = Rgb2YuvFactor_15[yuvMode][1]; dst[i].cb = (((bgr.r * t[0] + bgr.g * t[1] + bgr.b * t[2] + t[3]) >> 14) + 1) >> 1;
			t = Rgb2YuvFactor_15[yuvMode][2]; dst[i].cr = (((bgr.r * t[0] + bgr.g * t[1] + bgr.b * t[2] + t[3]) >> 14) + 1) >> 1;
		}

		return paletteLength * sizeof(EncodedPalette);
	}

	HashCode CompositionEncoder::GetImageHash(Rect hashRect)
	{
		return HashCode{};	//not implenmented.
	}

	HashCode CompositionEncoder::GetPaletteHash()
	{
		return HashCode{};	//not implenmented.
	}

	bool CompositionEncoder::IsImageBufferValid(int objId)
	{
		return false;	//not implenmented.
	}

	Rect CompositionEncoder::GetMaxAvailableRect(int objId)
	{
		auto &obj = cmpnObjs[objId];
		Border bMax = windows->windows[obj.windowRef].ToOuterBorder();
		Border bObj = obj.imageCrop.ToOuterBorder();

		for (int i = 0; i < cmpnObjsCount; ++i)
		{
			auto &iObj = cmpnObjs[i];
			Border bObjI = iObj.imageCrop.ToOuterBorder();

			if (iObj.windowRef == obj.windowRef)
			{
				bool tOvlp = (((bObjI.left - bMax.right) ^ (bObjI.right - bMax.left)) & SignBitMask) != 0;	//horizontal overlapped.
				if (tOvlp)
				{
					bMax.bottom = (bObj.bottom <= bObjI.top) ? min(bMax.bottom, bObjI.top) : bMax.bottom;
					bMax.top = (bObj.top >= bObjI.bottom) ? max(bMax.top, bObjI.bottom) : bMax.top;
				}

				tOvlp = (((bObjI.top - bMax.bottom) ^ (bObjI.bottom - bMax.top)) & SignBitMask) != 0;	//vertically overlapped.
				if (tOvlp)
				{
					bMax.right = (bObj.right <= bObjI.left) ? min(bMax.right, bObjI.left) : bMax.right;
					bMax.left = (bObj.left >= bObjI.right) ? max(bMax.left, bObjI.right) : bMax.left;
				}
			}
		}

		return Rect::FromOuterBorder(bMax);
	}

	Position CompositionEncoder::ArrangeRect(Rect content, Rect window, Size rectSize)
	{
		Size deltaSize = (rectSize - content.size);
		Offset tOfs{ deltaSize.w >> 1, deltaSize.h >> 1 };

		Rect tRect = { content.pos - tOfs, content.size + deltaSize };
		tOfs.dx = max(window.Left() - tRect.Left(), 0) + min(window.Right() - tRect.Right(), 0);
		tOfs.dy = max(window.Top() - tRect.Top(), 0) + min(window.Bottom() - tRect.Bottom(), 0);
		
		tRect = tRect + tOfs;
		return tRect.pos;
	}
}