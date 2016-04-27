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

/*----------------------------------------------------------------------------
* project is based on avs2bdnxml 2.08 (under GPLv3)
* Copyright (C) 2008-2013 Arne Bochem <avs2bdnxml at ps-auxw de>
*----------------------------------------------------------------------------*/

#pragma once

#include "A2PControllerBase.h"
#include "FrameStream.h"
#include "StreamEx.h"
#include "ProgressReporter.h"

namespace avs2pgs
{

	class A2PEncoder final
		: public A2PContext
	{
	public:
		A2PEncoder(A2PSettings const *settings, A2PLogger const *logger)
			: A2PContext(settings, logger) {}

		int Encode(FrameStream *input, StreamEx *output, ProgressReporter *reporter);
		
	};

}