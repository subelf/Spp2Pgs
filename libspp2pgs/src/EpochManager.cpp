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
#include "EpochManager.h"

namespace spp2pgs
{

	EpochManager::EpochManager(S2PContext const * context, Size frameSize, BdViFrameRate frameRate) :
		S2PControllerBase(context), minInterval((int)spp2pgs::GetFrameTimeStamp(1, frameRate)), frameSize(frameSize),
		binBoard(frameSize, this->Settings()->MaxImageBlockSize()), cropSizeMax(Size{ 0, 0 }), cache(context), isDequeuing(false)
	{}
	
	void EpochManager::EnqueueDisplaySet(DisplaySetThumb *thumb, IndexedImage *image)
	{
		if (isDequeuing)
		{
			throw EpochManagingException(EpochManagingExceptionType::DisplaySetEnqueueWhileDequeuing);
		}

		binBoard.MergeFrom(thumb->GetBinaryBoard());	//record pixels with data in any frame of this epoch
		cropSizeMax = Size::MinContainer(cropSizeMax, thumb->GetDataCrop().size);	//update size of largest possible image object

		CacheReceipt receipt = cache.Write(thumb->GetDataCrop(), image);	//cache to disk
		thumb->SetCacheReceipt(receipt);

		cmpnQueue.push(thumb);
		thumb = nullptr;
	}

	DisplaySetThumb* EpochManager::DequeueDisplaySet(IndexedImage *imageBuffer)
	{
		if (cmpnQueue.empty())
		{
			isDequeuing = false;
			cache.Reset();
			binBoard.Clear();
			cropSizeMax = Size{ 0, 0 };
			return nullptr;
		}

		isDequeuing = true;

		auto tThumb = cmpnQueue.front();
		cmpnQueue.pop();

		cache.Load(tThumb->GetCacheReceipt(), tThumb->GetDataCrop(), imageBuffer);
		return tThumb;
	}

	bool EpochManager::IsNewEpochStart(DisplaySetThumb *thumb)	//Estimate the possibility of a new epoch's start
	{
		if (cmpnQueue.empty())
		{
			return false;
		}

		int const &tVideoDuration = GetVideoInitDuration(this->frameSize.Area());
		int const &tRenderDuration = GetWindowDecodeDuration(this->frameSize.Area());

		int tDecodeDurationFull = GetObjectDecodeDuration(this->frameSize.Area(), false);
		tDecodeDurationFull = max(tDecodeDurationFull, tVideoDuration) + tRenderDuration;

		int const &tEraseLastDuration = GetWindowDecodeDuration(cropSizeMax.Area());

		auto const &last = *cmpnQueue.back();
		int const &tPossibleLastEpochEraseDuration = max(minInterval, tEraseLastDuration);
		__int64 const &tPossibleEts = max(last.GetPTS() + tPossibleLastEpochEraseDuration, last.GetETS());

		return (thumb->GetPTS() - tPossibleEts > tDecodeDurationFull);
	}
}