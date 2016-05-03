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

#include <S2PExceptions.h>

namespace Spp2PgsNet 
{
	using namespace System;

	template<typename T> inline T^ AssertClrArgumentNotNull(T^ argument)
	{
		if (argument == nullptr)
		{
			throw spp2pgs::S2PException(spp2pgs::S2PExceptionType::ArgumentNull, nullptr);
		}

		return argument;
	}

	template<typename T> inline T^ AssertClrArgumentNotNull(T^ argument, String ^name)
	{
		if (argument == nullptr)
		{
			throw gcnew ArgumentNullException(name);
		}

		return argument;
	}

	template<typename T> inline T* AssertClrArgumentNotNull(T* argument, String ^name)
	{
		if (argument == nullptr)
		{
			throw gcnew ArgumentNullException(name);
		}

		return argument;
	}
}