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

#include <S2PEncoder.h>
#include <FrameStream.h>

#include "IFrameStreamAdvisor.h"
#include "IProgressReporter.h"
#include "ClrSettings.h"
#include "ClrLogger.h"

namespace Spp2PgsNet
{
	using namespace spp2pgs;
	using namespace System;
	using namespace System::IO;
	using namespace Spp2PgsNet;

	public ref class Spp2Pgs : public IDisposable
	{
	public:
		Spp2Pgs(IS2PSettings ^settings, IS2PLogger ^logger);

		void Encode(Stream ^rgbaStream, IFrameStreamAdvisor ^advisor, Stream ^output, IProgressReporter ^reporter);
		void Encode(FileInfo ^avsFile, IFrameStreamAdvisor ^advisor, Stream ^output, IProgressReporter ^reporter);

		!Spp2Pgs();
		~Spp2Pgs();

	private:
		const IS2PSettings ^settings;
		IS2PLogger ^logger;

		ClrLogger *loggerNative = nullptr;
		ClrSettings *settingsNative = nullptr;
		spp2pgs::S2PEncoder *encoderNative = nullptr;

		bool disposed = false;

	};

}
