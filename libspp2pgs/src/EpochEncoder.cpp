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


#include "pch.h"
#include "EpochEncoder.h"

namespace spp2pgs
{

	EpochEncoder::EpochEncoder(S2PContext const *context, GxBufferManager * bufMgr, BinaryBoard const * binBoard, Size const & size) :
		S2PControllerBase(context), 
		windows(EpochEncoder::CreateWindows(binBoard)), bufMgr(bufMgr)
	{
		bufMgr->Clear();

		if (!this->Settings()->IsForcingTmtCompat())
		{
			bufMgr->SetObjectDataSizeLimit(size);
		}
	}


	EpochEncoder::~EpochEncoder()
	{
	}

	
	WindowsDescriptor EpochEncoder::CreateWindows(BinaryBoard const * binBoard)
	{
		WindowsDescriptor wndDesc;
		wndDesc.count = binBoard->QuickSplit(wndDesc.windows);

		return wndDesc;
	}

	CompositionBuffer const * EpochEncoder::EncodeComposition(DisplaySetThumb const * thumb, IndexedImage const * image)
	{
		bufMgr->ReleaseEncodeBuffer();

		CompositionEncoder cmpnEnc { this->context, &windows, bufMgr };

		return cmpnEnc.Encode(thumb, image);
	}
}