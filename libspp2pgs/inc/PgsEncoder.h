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

#include <windows.h>

#include "S2PControllerBase.h"
#include "StreamEx.h"
#include "PgsWriter.h"
#include "GraphicalTypes.h"
#include "BlurayCommon.h"
#include "EpochManager.h"
#include "GxBufferManager.h"

namespace spp2pgs
{

	class PgsEncoder final
		: public S2PControllerBase
	{
	public:
		PgsEncoder(S2PContext const *context, StreamEx *output, Size frameSize, BdViFrameRate frameRate);
		~PgsEncoder();

		inline Size GetFrameSize() { return this->frameSize; }
		inline BdViFrameRate GetFrameRate() { return this->frameRate; }

		void RegistFrame(BgraFrame *frame, int length) throw(TempOutputException, ImageOperationException);

		void EncodeEpoch();

	private:
		Size const frameSize;
		BdViFrameRate frameRate;

		std::unique_ptr<PgsWriter> output;
		std::unique_ptr<GxBufferManager> bufMgr;

		EpochManager epochManager;
		IndexedImage imageBuffer;

		int epochCount = 0;
	};

}