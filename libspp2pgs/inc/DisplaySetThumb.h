/*----------------------------------------------------------------------------
* spp2pgs - Generates BluRay PG Stream from RGBA AviSynth scripts
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

#include "GraphicalTypes.h"
#include "BinaryBoard.h"
#include "EpochCache.h"

namespace spp2pgs
{

	class DisplaySetThumb final
	{
	public:
		DisplaySetThumb(BgraFrame *frameStart, int length, BdViFrameRate frameRate, int maxBlockSize);
		~DisplaySetThumb();

		inline void SetCacheReceipt(CacheReceipt receipt) { cacheReceipt = receipt; }
		inline CacheReceipt GetCacheReceipt() const { return cacheReceipt; }
		inline const BinaryBoard * GetBinaryBoard() const { return &(this->binBoard); }
		inline const Rect GetDataCrop() const { return this->dataCrop; }
		inline const __int64 GetPTS() const { return this->pts; }
		inline const __int64 GetETS() const { return this->ets; }
		
	private:
		__int64 pts, ets;	//presenting time & ereasing time
		BinaryBoard binBoard;
		Rect dataCrop;
		CacheReceipt cacheReceipt;
	};

}