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
#include "PgsWriter.h"

namespace avs2pgs
{

	int const PgsWriter::MinPtsIntervalTable[] = {
		0, 3750, 3750, 3600, 3000, 3000, 1800, 1500, 1500
	};

	PgsWriter::PgsWriter(A2PContext const *context, Size videoSize, BdViFrameRate frameRate, StreamEx* output) :
		A2PControllerBase(context), videoSize(videoSize), frameRate(frameRate), output(output), clockPerFrame(avs2pgs::ClockPerSecond / avs2pgs::GetFrameRate(frameRate)),
		compositionCount(0), isEpochStart(true), minInterval(MinPtsIntervalTable[(int)frameRate]), lastDecEnd(MININT64),
		lastCmpn({ MININT64, MININT64, nullptr, nullptr, 0, nullptr })
	{
		buffer[0] = 'P';
		buffer[1] = 'G';

		this->Log(A2PLogger::info + A2PLogger::normal, _T("PgsWriter Created.\n"));
	}


	PgsWriter::~PgsWriter()
	{
	}


	void PgsWriter::StartEpoch(WindowsDescriptor const * wndDesc)
	{
		this->wndDesc = wndDesc;
		isEpochStart = true;
		lastCmpn.cmpnObjsCount = 0;
		lastDecEnd = MININT64;
	}

	void PgsWriter::WriteComposition(CompositionBuffer const * composition)
	{
		if (composition->windows != this->wndDesc)
		{
			this->Log(A2PLogger::warning + A2PLogger::high, _T("Encountered an invalid composition at PTS=%lld.\n"
				"\tComposition is not using same window with current Epoch.\n"), composition->pts);
			return;
		}

		//buffer initialization duration
		int tDuration = isEpochStart ? GetVideoInitDuration(videoSize.Area()) : 1;

		__int64 pts = composition->pts;
		int const& decDur = composition->EstimateDecodeDuration(tDuration);
		int const& ersDur = wndDesc->EstimateDecodeDuration();

		if (this->Settings()->IsForcingEpochZeroStart() && (compositionCount == 0))	//first Epoch
		{
			if (pts > max(decDur, minInterval) + minInterval)
			{
				this->WriteZeroStartEpoch();
			}
			else
			{
				pts = 0;
			}
		}

		__int64 dts = pts - decDur;
		__int64 endPts = dts;

		if (!this->Settings()->IsForcingTmtCompat())
		{
			int minInt = minInterval;
			if (!IsSameObjectToPrev(composition))
			{
				minInt = max(minInt, ersDur);	//reserve interval for displaying and erease the last composition
			}
			if (!isEpochStart && (pts < lastCmpn.pts + minInt || dts < lastDecEnd))	//and decoding cannot start while last composition is not ended.
			{
				this->Log(A2PLogger::warning + A2PLogger::high, _T("Ignored a composition at PTS=%lld ~ ETS=%lld.\n"
					"\tNo enough time for decoding and draw this componsition.\n"), composition->pts, composition->ets);
				return IgnoreComposition(composition->pts, composition->ets);
			}
		}

		if (!isEpochStart && pts > lastCmpn.ets + clockPerFrame)
		{
			this->TryInsertEraserBefore(dts);
		}

		FillPackageHead(pts, dts);
		WritePCS(composition);

		if (wndDesc->count > 0)
		{
			pts = pts - wndDesc->EstimateDecodeDuration();
			pts = (pts == dts) ? pts + 1 : pts;
			FillPackageHead(pts, dts);
			WriteWDS();
		}

		if (composition->encodedPalette != nullptr && composition->encodedPaletteLength > 0)
		{
			FillPackageHead(dts, 0);
			WritePDS(composition);
		}

		tDuration = 0;
		for (int i = 0; i < composition->cmpnObjsCount; ++i)
		{
			auto const& tCmpnObj = composition->cmpnObjs[i];
			if (tCmpnObj.objectRef == nullptr)	//not a valid composition(image object)
			{
				continue;
			}

			int tEndDuration = tDuration + tCmpnObj.EstimateDecodeDuration();
			endPts = dts + tEndDuration;

			if (tCmpnObj.encodedObject != nullptr && tCmpnObj.encodedObjectLength > 0)	//if containing new image object
			{
				FillPackageHead(endPts, dts + tDuration);
				WriteODS(&tCmpnObj);
			}

			tDuration = tEndDuration;
		}

		FillPackageHead(endPts, 0);
		WriteEND();
		
		lastDecEnd = endPts;
		lastCmpn = *composition;
		lastCmpn.paletteRef = nullptr;
		for (int i = 0; i < lastCmpn.cmpnObjsCount; ++i)
		{
			lastObjBufId[i] = lastCmpn.cmpnObjs[i].objectRef->GetId();
			lastObjBufVersion[i] = lastCmpn.cmpnObjs[i].objectRef->GetVersion();

			lastCmpn.cmpnObjs[i].objectRef = nullptr;
		}
	}

	void PgsWriter::EndEpoch()
	{
		WriteEraser(lastCmpn.ets);

		isEpochStart = true;
		wndDesc = nullptr;
	}

	void PgsWriter::WriteEraser(__int64 ets)
	{
		if (ets < lastCmpn.ets)
		{
			this->Log(A2PLogger::warning + A2PLogger::high, _T("Failed to erase at ETS=%lld.\n"
				"\tValue is below ETS of previous composition.\n"), ets);
			return;
		}

		int const& eraseDuration = wndDesc->EstimateDecodeDuration();
		__int64 pts = ets;

		if (!this->Settings()->IsForcingTmtCompat())
		{
			double const & minIntervalFramed = ceil(eraseDuration / clockPerFrame) * clockPerFrame;
			double const & lastPosibleEts = max(ets, lastCmpn.pts + minIntervalFramed);	//reserve time for erasing
			int const &lastPosibleEtsFrame = (int)(round(lastPosibleEts / clockPerFrame));
			pts = avs2pgs::GetFrameTimeStamp(lastPosibleEtsFrame, frameRate);	//update ets to a reasonable value
		}

		__int64 dts = pts - eraseDuration - 1;

		FillPackageHead(pts, dts);
		WritePCS(nullptr);

		FillPackageHead(dts + 1, dts);
		WriteWDS();

		FillPackageHead(dts, 0);
		WriteEND();

		lastCmpn.cmpnObjsCount = 0;
		lastDecEnd = MININT64;
	}

	void PgsWriter::IgnoreComposition(__int64 pts, __int64 ets)
	{
		if (!isEpochStart && pts > lastCmpn.ets + clockPerFrame)
		{
			this->TryInsertEraserBefore(pts);
		}

		lastCmpn.ets = ets;
	}

	void PgsWriter::TryInsertEraserBefore(__int64 dts)
	{
		if (lastCmpn.cmpnObjsCount > 0)
		{
			int eraseDuration = wndDesc->EstimateDecodeDuration();

			double const & minIntervalFramed = ceil(eraseDuration / clockPerFrame) * clockPerFrame;
			double const & lastPosibleEts = max(lastCmpn.ets, lastCmpn.pts + minIntervalFramed);

			if (dts > lastPosibleEts + minIntervalFramed)
			{
				WriteEraser(lastCmpn.ets);
			}
		}
	}

	void PgsWriter::WritePCS(CompositionBuffer const * composition)
	{
		int tOfs = 0;

		BdCompositionType cmpnType = BdCompositionType::NormalCase;
		if (composition != nullptr)
		{
			for (int i = 0; i < composition->cmpnObjsCount; ++i)
			{
				if (composition->cmpnObjs[i].encodedObject != nullptr && composition->cmpnObjs[i].encodedObjectLength > 0)	//contains new img-obj
				{
					cmpnType = isEpochStart ? BdCompositionType::EpochStart : BdCompositionType::AcquisitionPoint;
					break;
				}
			}
		}
		isEpochStart = false;

		bool fPaletteUpdate =
			(cmpnType == BdCompositionType::NormalCase) &&	//no new img-obj,
			(composition != nullptr) &&	//and not merely a erasing PCS;
			IsSameObjectToPrev(composition) &&	//everything is same to the last one,
			(composition->encodedPalette != nullptr) &&	//except the palette,
			(composition->encodedPaletteLength > 0);	//then set up the palette update flag

		//video_descriptor	//5B
		tOfs += FillSegmentData(tOfs, 16, videoSize.w);
		tOfs += FillSegmentData(tOfs, 16, videoSize.h);
		tOfs += FillSegmentData(tOfs, 4, static_cast<unsigned __int8>(frameRate));
		tOfs += FillSegmentData(tOfs, 4, 0);	//reserved.

		//compositon_discriptor
		tOfs += FillSegmentData(tOfs, 16, compositionCount++);
		tOfs += FillSegmentData(tOfs, 2, static_cast<unsigned __int8>(cmpnType));
		tOfs += FillSegmentData(tOfs, 6, 0);	//reserved.

		//palette_update_flag
		tOfs += FillSegmentData(tOfs, 1, fPaletteUpdate);
		tOfs += FillSegmentData(tOfs, 7, 0);	//reserved.
		//palette_id_ref
		tOfs += FillSegmentData(tOfs, 8, (composition == nullptr) ? 0 : composition->paletteRef->GetId());

		int tCmpnCount = (composition == nullptr) ? 0 : composition->cmpnObjsCount;
		//number_of_composition_objects
		tOfs += FillSegmentData(tOfs, 8, tCmpnCount);
		for (int i = 0; i < tCmpnCount; ++i)
		{
			auto &cmpnObj = composition->cmpnObjs[i];
			auto objRef = cmpnObj.objectRef;
			bool isCropped = (cmpnObj.imageCrop.pos != cmpnObj.objectPos) || (cmpnObj.imageCrop.size != (*objRef)->dataSize);

			//auto offset = cmpnObj.imageCrop.pos - wndDesc->windows[cmpnObj.windowRef].pos;

			//composition_object
			tOfs += FillSegmentData(tOfs, 16, objRef->GetId());
			tOfs += FillSegmentData(tOfs, 8, cmpnObj.windowRef);
			tOfs += FillSegmentData(tOfs, 1, isCropped);
			tOfs += FillSegmentData(tOfs, 1, 0);	//forced on;
			tOfs += FillSegmentData(tOfs, 6, 0);	//reserved.

			tOfs += FillSegmentData(tOfs, 16, cmpnObj.imageCrop.x);
			tOfs += FillSegmentData(tOfs, 16, cmpnObj.imageCrop.y);
			if (isCropped)
			{
				auto offset = cmpnObj.imageCrop.pos - cmpnObj.objectPos;	//not verified.
				assert(offset.dx >= 0 && offset.dy >= 0);

				//cropping_rectangle
				tOfs += FillSegmentData(tOfs, 16, offset.dx);
				tOfs += FillSegmentData(tOfs, 16, offset.dy);
				tOfs += FillSegmentData(tOfs, 16, cmpnObj.imageCrop.w);
				tOfs += FillSegmentData(tOfs, 16, cmpnObj.imageCrop.h);
			}
		}

		assert(!(tOfs & 0x07));

		FillSegmentHead(BdGxSegmentType::PCS, tOfs >> 3);
		WriteSegmentData(tOfs >> 3);
	}

	void PgsWriter::WriteWDS()
	{
		int tOfs = 0;

		//number_of_windows
		tOfs += FillSegmentData(tOfs, 8, wndDesc->count);

		for (int i = 0; i < wndDesc->count; ++i)
		{
			auto &wnd = wndDesc->windows[i];

			//window
			tOfs += FillSegmentData(tOfs, 8, i);
			tOfs += FillSegmentData(tOfs, 16, wnd.x);
			tOfs += FillSegmentData(tOfs, 16, wnd.y);
			tOfs += FillSegmentData(tOfs, 16, wnd.w);
			tOfs += FillSegmentData(tOfs, 16, wnd.h);
		}

		assert(!(tOfs & 0x07));

		FillSegmentHead(BdGxSegmentType::WDS, tOfs >> 3);
		WriteSegmentData(tOfs >> 3);
	}

	void PgsWriter::WritePDS(CompositionBuffer const * composition)
	{
		int tOfs = 0;
		
		auto & palRef = *composition->paletteRef;

		//palette_id
		tOfs += FillSegmentData(tOfs, 8, palRef.GetId());
		tOfs += FillSegmentData(tOfs, 8, palRef.GetVersion());

		assert(!(tOfs & 0x07));

		FillSegmentHead(BdGxSegmentType::PDS, (tOfs >> 3) + composition->encodedPaletteLength);
		WriteSegmentData(tOfs >> 3);
		WriteAdditionalData(composition->encodedPalette, 0, composition->encodedPaletteLength);
	}


	void PgsWriter::WriteODS(CompositionObjectDescriptor const * object)
	{
		int tOfs = 0;
		bool isFirstFrag = true;
		bool isLastFrag = false;
		int tLeftLen = object->encodedObjectLength;

		auto & objRef = *object->objectRef;
		//object_id
		tOfs += FillSegmentData(tOfs, 16, objRef.GetId());
		tOfs += FillSegmentData(tOfs, 8, objRef.GetVersion());
		
		int tOfsFrag = tOfs;
		int tDataWritten = 0;
		
		while (tLeftLen > 0)
		{
			tOfs = tOfsFrag;

			int tDataLen = (isFirstFrag ? 65508 : 65515);
			isLastFrag = (tLeftLen <= tDataLen);
			tDataLen = isLastFrag ? tLeftLen : tDataLen;

			//sequence_first_segment_flag
			tOfs += FillSegmentData(tOfs, 1, isFirstFrag);
			tOfs += FillSegmentData(tOfs, 1, isLastFrag);
			tOfs += FillSegmentData(tOfs, 6, 0);	//reserved.

			if (isFirstFrag)
			{
				//segment_data_size
				tOfs += FillSegmentData(tOfs, 24, 4 + tLeftLen);	//length including size info
				tOfs += FillSegmentData(tOfs, 16, objRef->dataSize.w);
				tOfs += FillSegmentData(tOfs, 16, objRef->dataSize.h);
			}

			assert(!(tOfs & 0x07));

			FillSegmentHead(BdGxSegmentType::ODS, (tOfs >> 3) + tDataLen);
			WriteSegmentData(tOfs >> 3);
			WriteAdditionalData(object->encodedObject, tDataWritten, tDataLen);

			tDataWritten += tDataLen;
			tLeftLen -= tDataLen;

			isFirstFrag = false;
		}
	}

	void PgsWriter::WriteEND()
	{
		FillSegmentHead(BdGxSegmentType::END, 0);
		WriteSegmentData(0);
	}

	void PgsWriter::WriteSegmentData(unsigned short length)
	{
		auto tLen = SegmentHeadLength + length;
		output->Write(buffer, 0, tLen);
	}

	void PgsWriter::WriteAdditionalData(unsigned __int8 const * buffer, int index, unsigned short length)
	{
		output->Write(buffer, index, length);
	}

	void PgsWriter::WriteZeroStartEpoch()
	{
		auto tCurWnd = this->wndDesc;

		Rect const &tRect = { 0, 0, 1, 1 };

		WindowsDescriptor tWnd{ 1, { tRect } };

		GxBuffer<PaletteBuffer> tPalette {0};
		tPalette.UpdateVersion();
		tPalette.AddRef();

		unsigned char tPaletteDataBuffer[] = {0, 16, 128, 128, 0};

		GxBuffer<ObjectBuffer> tObject {0};
		tObject.UpdateVersion();
		tObject.AddRef();
		tObject->dataSize = tRect.size;

		unsigned char tImageDataBuffer[] = { 0, 1, 0, 0 };

		CompositionBuffer tComposition{ 0, minInterval,
			&tWnd,
			&tPalette, sizeof(tPaletteDataBuffer), tPaletteDataBuffer,
			1, {{0, tRect.pos, tRect, &tObject, sizeof(tImageDataBuffer), tImageDataBuffer }}
		};

		this->wndDesc = &tWnd;
		this->WriteComposition(&tComposition);
		this->EndEpoch();

		this->StartEpoch(tCurWnd);
	}

}