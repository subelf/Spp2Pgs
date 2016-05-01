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

#include "S2PContext.h"
#include "S2PExceptions.h"


namespace spp2pgs
{

	class S2PControllerBase
	{
	public:
		S2PControllerBase(S2PContext const * context)
			:context(AssertArgumentNotNull(context))
		{
		}

	public:

		virtual ~S2PControllerBase() {};

	protected:

		void Log(int level, TCHAR* fmt, ...) const
		{
			va_list argp;
			va_start(argp, fmt);
			this->context->Logger()->Vlog(level, fmt, argp);
			va_end(argp);
		}

		S2PSettings const * Settings()
		{
			return context->Settings();
		}

		S2PContext const * const context;
		
	};

}