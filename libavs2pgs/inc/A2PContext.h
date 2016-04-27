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

#include "A2PLogger.h"
#include "A2PSettings.h"
#include "Global.h"

namespace avs2pgs
{

	class A2PContext
	{
	public:
		A2PContext(A2PSettings const * settings, A2PLogger const * logger)
			:settings(AssertArgumentNotNull(settings)), logger(AssertArgumentNotNull(logger))
		{
		}

	public:
		~A2PContext() {};

		A2PSettings const * Settings() const { return this->settings; }
		A2PLogger const * Logger() const { return this->logger; }

	private:
		A2PSettings const * settings;
		A2PLogger const * logger;
	};

}