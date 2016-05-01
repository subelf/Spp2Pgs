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

#include "libspp2pgs.h"
#include "S2PEncoder.h"
#include "GraphicalTypes.h"
#include "BlurayCommon.h"
#include "FrameStreamAdvisor.h"
#include "S2PExceptions.h"
#include "BgraFrame.h"
#include "PgsEncoder.h"

namespace spp2pgs
{

	int Avs2Pgs(FrameStream *input, StreamEx *output, ProgressReporter *reporter)
	{
		S2PDefaultSettings tSettings{};
		S2PNullLogger tLogger{};

		S2PEncoder tEncoder{ &tSettings, &tLogger };
		return tEncoder.Encode(input, output, reporter);
	}

}