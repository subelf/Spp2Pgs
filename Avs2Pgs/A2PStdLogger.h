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

#include <A2PLogger.h>

using namespace avs2pgs;

class A2PStdLogger final:
	public A2PLogger
{
public:
	A2PStdLogger(int level) : logLevel(level) {}
	A2PStdLogger() : A2PStdLogger(A2PLogger::info) {}
	~A2PStdLogger() {}

	void Vlog(int level, const TCHAR *fmt, va_list valist) const throw();
	void SetLogLevel(int level) { this->logLevel = level; }

private:
	int logLevel;
};

