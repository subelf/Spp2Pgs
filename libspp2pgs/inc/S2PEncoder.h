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

/*----------------------------------------------------------------------------
* project is based on avs2bdnxml 2.08 (under GPLv3)
* Copyright (C) 2008-2013 Arne Bochem <avs2bdnxml at ps-auxw de>
*----------------------------------------------------------------------------*/

#pragma once

#include "S2PControllerBase.h"
#include "FrameStream.h"
#include "StreamEx.h"
#include "ProgressReporter.h"

namespace spp2pgs
{

	class S2PEncoder final
		: public S2PContext
	{
	public:
		S2PEncoder(S2PSettings const *settings, S2PLogger const *logger)
			: S2PContext(settings, logger) {}

		int Encode(FrameStream *input, StreamEx *output, ProgressReporter *reporter);
		
	};

}