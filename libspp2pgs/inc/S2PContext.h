/*----------------------------------------------------------------------------
* spp2pgs - Generates BluRay PG Stream from RGBA AviSynth scripts
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

#include "S2PLogger.h"
#include "S2PSettings.h"
#include "Global.h"

namespace spp2pgs
{

	class S2PContext
	{
	public:
		S2PContext(S2PSettings const * settings, S2PLogger const * logger)
			:settings(AssertArgumentNotNull(settings)), logger(AssertArgumentNotNull(logger))
		{
		}

	public:
		~S2PContext() {};

		S2PSettings const * Settings() const { return this->settings; }
		S2PLogger const * Logger() const { return this->logger; }

	private:
		S2PSettings const * settings;
		S2PLogger const * logger;
	};

}