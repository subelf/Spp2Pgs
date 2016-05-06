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

#include <memory>
#include <strmif.h>
#include <VSSppfApi.h>

#include "S2PExceptions.h"
#include "BlurayCommon.h"
#include "GraphicalTypes.h"
#include "StreamEx.h"
#include "BgraFrame.h"

namespace spp2pgs
{
	template<typename T> inline T* AssertArgumentNotNull(T* argument)
	{
		if (argument == nullptr)
		{
			throw S2PException(S2PExceptionType::ArgumentNull, nullptr);
		}

		return argument;
	}

	inline void MethodNotImplenmented()
	{
		throw S2PException(S2PExceptionType::NotImplenmented, nullptr);
	}

	inline __int64 GcdOf(__int64 a, __int64 b){
		for (;;)
		{
			__int64 c = a % b;
			if (!c)
				return b;
			a = b;
			b = c;
		}
	}

	inline __int64 GdBelow(__int64 x, __int64 limit){
		limit = min(x, limit);
		while (x % limit) limit--;
		return limit;
	}

	inline int GetDecodeDuration(int area, int const millionRd){
		const int & tRd = millionRd * 100;
		int tRet = (area * 72 - 1) / tRd + 1;

		return tRet;
	}

	inline int GetVideoInitDuration(int area)	//layer allocating
	{
		return GetDecodeDuration(area, 256);
	}

	inline int GetObjectDecodeDuration(int area, bool isIg)	//RLE decoding
	{
		int millionRd = isIg ? 64 : 128;
		return GetDecodeDuration(area, millionRd);
	}

	inline int GetWindowDecodeDuration(int area)	//windows erasing
	{
		return GetDecodeDuration(area, 256);
	}

	Size const VideoFormatTable[] = {
		{ 0, 0 }, { 720, 480 }, { 720, 576 },
		{ 720, 480 }, { 1920, 1080 }, { 1280, 720 },
		{ 1920, 1080 }, { 720, 576 }
	};
	inline Size GetFrameSize(BdViFormat format)
	{
		return VideoFormatTable[(int)format];
	}

	struct FrameRateInfo
	{
		int framesPerUnit, secondsPerUnit;
		inline double GetFrameRate() const { return framesPerUnit /(double) secondsPerUnit; }
	};

	int const ClocksPerSecond = 90000uL;
	FrameRateInfo const FrameRateTable[] = {
		{ 0, 1000 }, { 24000, 1001 }, { 24000, 1000 },
		{ 25000, 1000 }, { 30000, 1001 }, { 30000, 1000 },
		{ 50000, 1000 }, { 60000, 1001 }, { 60000, 1000 }
	};
	inline double GetFramePerSecond(BdViFrameRate rate){
		auto const& frameRateInfo = FrameRateTable[(int)rate];
		return frameRateInfo.GetFrameRate();
	}
	inline BdViFrameRate GuessBdFrameRateFrom(double rate){
		int const &count = _countof(FrameRateTable);
		double tDevMax = 0.01;
		BdViFrameRate tRet = BdViFrameRate::Unknown;

		for (int i = 0; i < count; ++i)
		{
			double const& tDev = abs(FrameRateTable[i].GetFrameRate() - rate);
			if (tDev < tDevMax)
			{
				tRet = static_cast<BdViFrameRate>(i);
				tDevMax = tDev;
			}
		}

		return tRet;
	}
	inline __int64 GetFrameTimeStamp(int index, BdViFrameRate rate){
		auto const& frameRateInfo = FrameRateTable[(int)rate];
		__int64 const& frames = index;
		int const& kFramesPerUnit = frameRateInfo.framesPerUnit / 1000;
		int const& kClocksPerSecond = ClocksPerSecond / 1000;
		int const& secondsPerUnit = frameRateInfo.secondsPerUnit;
		
		return frames * kClocksPerSecond * secondsPerUnit / kFramesPerUnit;
	}

	long long const RefTimeTps = 10000000;	//100ns per tick of RefTime;

	inline REFERENCE_TIME GetRefTimeOfFrame(int frameIndex, BdViFrameRate rate)
	{
		auto const& frameRateInfo = FrameRateTable[(int)rate];
		long long const& frames = frameIndex;
		int const& FramesPerUnit = frameRateInfo.framesPerUnit;
		long long const& TicksPerUnit = RefTimeTps * frameRateInfo.secondsPerUnit;

		return frames * TicksPerUnit / FramesPerUnit;
	}

	inline int GetFirstFrameFromRT(REFERENCE_TIME rt, BdViFrameRate rate)
	{
		auto const& frameRateInfo = FrameRateTable[(int)rate];
		long long const& refTimeTicks = rt;
		int const& FramesPerUnit = frameRateInfo.framesPerUnit;
		long long const& TicksPerUnit = RefTimeTps * frameRateInfo.secondsPerUnit;

		return static_cast<int>((refTimeTicks * FramesPerUnit - 1) / TicksPerUnit + 1);
	}

	inline SubPicAlfaDesc DescribeTargetBuffer(BgraFrame const *frame)
	{
		SubPicAlfaDesc spad;
		spad.bits = reinterpret_cast<BYTE*>(frame->GetDataBuffer());
		spad.w = frame->GetWidth();
		spad.h = frame->GetHeight();
		spad.pitch = frame->GetStride();
		spad.vidrect = RECT{ 0, 0, frame->GetWidth(), frame->GetHeight() };
		return spad;
	}

	std::unique_ptr<StreamEx> OpenTempFile(unsigned __int64 requireFreeSpace);

	void WriteBE(unsigned __int8 * buffer, int index, unsigned __int8 length, unsigned __int64 value);
	unsigned __int64 ReadBE(unsigned __int8 const * buffer, int index, unsigned __int8 length);

}