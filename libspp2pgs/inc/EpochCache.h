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

#include <memory>

#include "S2PControllerBase.h"
#include "IndexedImage.h"
#include "StreamEx.h"


namespace spp2pgs
{
	class CacheReceipt final
	{
	public:
		CacheReceipt(__int64 position) : position(position) {}
		~CacheReceipt(){}

		inline __int64 GetPosition() { return position; }
	private:
		__int64 position;
	};

	class EpochCache final
		:public S2PControllerBase
	{
	public:
		EpochCache(S2PContext const * context);
		~EpochCache();

		//Cache an indexed image into temparary file.
		CacheReceipt Write(Rect dataCrop, IndexedImage *image);
		void Load(CacheReceipt receipt, Rect dataCrop, IndexedImage *image);

		inline void Reset() { tempFile->SetPosition(0); }

	private:
		std::unique_ptr<StreamEx> tempFile;
	};


}