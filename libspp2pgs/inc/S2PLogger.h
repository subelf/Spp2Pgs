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

#pragma once

#include <stdarg.h>
#include <tchar.h>
#include <Windows.h>

namespace avs2pgs
{

	class A2PLogger
	{
	protected:
		A2PLogger() {};

	public:
		virtual ~A2PLogger() {};
		void Log(int level, const TCHAR *fmt, ...) const throw()
		{
			va_list argp;
			va_start(argp, fmt);
			this->Vlog(level, fmt, argp);
			va_end(argp);
		}
		virtual void Vlog(int level, const TCHAR *fmt, va_list valist) const throw() = 0;

	public:
		static int const error = 0, warning = 64, info = 128, all = MAXINT;
		static int const high = 0, normal = 16, low = 32, verbose = 48;
	};


	class A2PNullLogger final
		: public A2PLogger
	{
	public:
		void Vlog(int level, const TCHAR* fmt, va_list valist) const {}
	};

}