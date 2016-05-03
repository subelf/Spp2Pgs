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

#pragma warning( disable : 4290 )

#ifdef _WIN64
#define _WIN32_WINNT 0x0600
#else
#define _WIN32_WINNT 0x0502
#endif

#include <WinSDKVer.h>

#include <afx.h>
#include <combaseapi.h>
#include <strmif.h>
#include <VSSppfApi.h>

#include "Spp2PgsNetGlobal.h"