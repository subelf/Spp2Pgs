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
#include "DisplaySetThumb.h"
#include "BgraFrame.h"

namespace avs2pgs
{

	DisplaySetThumb::DisplaySetThumb(BgraFrame *frameStart, int length, BdViFrameRate frameRate, int maxBlockSize)
		:pts(avs2pgs::GetFrameTimeStamp(frameStart->GetFrameIndex(), frameRate)),
		ets(avs2pgs::GetFrameTimeStamp(frameStart->GetFrameIndex() + length, frameRate)),
		binBoard(frameStart, maxBlockSize), dataCrop(binBoard.QuickCrop()),
		cacheReceipt(-1)
	{
	}


	DisplaySetThumb::~DisplaySetThumb()
	{
	}
}