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

#include <S2PLogger.h>

#include "IS2PLogger.h"

namespace spp2pgs
{

	using namespace Spp2PgsNet;

	class ClrLogger final :
		public S2PLogger
	{
	public:
		ClrLogger(IS2PLogger ^logger) : loggerNet(AssertClrArgumentNotNull(logger)) {}
		~ClrLogger() {}

		void Vlog(int level, const TCHAR *fmt, va_list valist) const throw()
		{
			TCHAR buffer[MaxLineLen];
			_vstprintf_s(buffer, fmt, valist);
			loggerNet->Vlog(level, gcnew String(buffer));
		}

	private:
		gcroot<IS2PLogger ^> loggerNet;

		static int const MaxLineLen = 1024;

	};

}