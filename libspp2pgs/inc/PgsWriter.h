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

#include "S2PControllerBase.h"
#include "GxBufferManager.h"
#include "BlurayCommon.h"
#include "GraphicalTypes.h"
#include "StreamEx.h"

namespace spp2pgs
{
	
	class PgsWriter final
		: public S2PControllerBase
	{
	public:
		PgsWriter(S2PContext const *context, Size videoSize, BdViFrameRate frameRate, StreamEx* output);
		~PgsWriter();

		void StartEpoch(WindowsDescriptor const * wndDesc);
		void WriteComposition(CompositionBuffer const * composition);
		void IgnoreComposition(__int64 pts, __int64 dts);
		void EndEpoch();

	private:
		static int const MaxSegmentLength = 65536;
		static unsigned short const PackageHeadLength = 2;	//'PG'
		static unsigned short const PackagePTSLength = 4;
		static unsigned short const PackageDTSLength = 4;
		static unsigned short const SegmentTypeLength = 1;	//SegmentType
		static unsigned short const SegmentInfoLength = 2;	//SegmentLength
		static unsigned short const SegmentHeadLength = 
			PackageHeadLength + PackagePTSLength + PackageDTSLength + SegmentTypeLength + SegmentInfoLength;

		static int const MinPtsIntervalTable[];

		int const minInterval;
		double const clockPerFrame;

		Size videoSize;
		BdViFrameRate frameRate;
		StreamEx* output;

		unsigned __int8 buffer[MaxSegmentLength];
		unsigned __int8 * const pkgBuf = buffer + PackageHeadLength;
		unsigned __int8 * const segBuf = buffer + SegmentHeadLength;
		
		int compositionCount;
		bool isEpochStart;
		CompositionBuffer lastCmpn;
		__int64 lastDecEnd;
		int lastObjBufId[CompositionObjectCountMax];
		int lastObjBufVersion[CompositionObjectCountMax];

		WindowsDescriptor const * wndDesc;

		void TryInsertEraserBefore(__int64 pts);
		void WriteEraser(__int64 ets);
		
		void WritePCS(CompositionBuffer const * composition);
		void WriteWDS();
		void WritePDS(CompositionBuffer const * composition);
		void WriteODS(CompositionObjectDescriptor const * object);
		void WriteEND();

		void WriteZeroStartEpoch();

		inline unsigned __int8 FillPackageData(int index, unsigned __int8 length, unsigned __int64 value) {
			WriteBE(pkgBuf, index, length, value);
			return length;
		}
		inline unsigned __int8 FillSegmentData(int index, unsigned __int8 length, unsigned __int64 value) {
			WriteBE(segBuf, index, length, value);
			return length;
		}

		bool IsSameObjectToPrev(CompositionBuffer const * composition){
			if (composition->cmpnObjsCount != lastCmpn.cmpnObjsCount) return false;
			for (int i = 0; i < lastCmpn.cmpnObjsCount; ++i)
			{
				auto &cur = composition->cmpnObjs[i];
				auto &prev = lastCmpn.cmpnObjs[i];
				bool isSame = (cur.windowRef == prev.windowRef);
				isSame &= (cur.objectPos == prev.objectPos);
				isSame &= (cur.imageCrop == prev.imageCrop);
				isSame &= (cur.objectRef->GetId() == lastObjBufId[i]);
				isSame &= (cur.objectRef->GetVersion() == lastObjBufVersion[i]);
				if (!isSame) return false;
			}
			return true;
		}

		void TryInsertEraser(__int64 pts);

		void FillPackageHead(__int64 pts, __int64 dts) {
			int tOfs = 0;
			tOfs += FillPackageData(tOfs, PackagePTSLength * 8, pts);
			FillPackageData(tOfs, PackageDTSLength * 8, dts);
		}
		void FillSegmentHead(BdGxSegmentType type, unsigned short length) {
			int tOfs = (PackagePTSLength + PackageDTSLength) * 8;
			tOfs += FillPackageData(tOfs, SegmentTypeLength * 8, static_cast<unsigned __int8>(type));
			FillPackageData(tOfs, SegmentInfoLength * 8, length);
		}

		void WriteSegmentData(unsigned short length);
		void WriteAdditionalData(unsigned __int8 const * buffer, int index, unsigned short length);
	};

}