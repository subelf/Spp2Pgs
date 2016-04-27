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

#include "stdafx.h"

#include <memory>
#include <BgraRawStream.h>
#include <BgraAvsStream.h>
#include <libavs2pgs.h>
#include <vcclr.h>

#include "ClrStreamEx.h"
#include "ClrAdvisor.h"
#include "ClrReporter.h"
#include "Avs2PgsNet.h"

namespace Avs2PgsNet
{
	Avs2Pgs::Avs2Pgs(IA2PSettings ^ settings, IA2PLogger ^ logger)
		: settings(AssertClrArgumentNotNull(settings, "settings")), logger(AssertClrArgumentNotNull(logger, "logger"))
	{
		settingsNative = new ClrSettings{ settings };
		loggerNative = new ClrLogger{ logger };
		encoderNative = new avs2pgs::A2PEncoder{ settingsNative, loggerNative };
	}

	void Avs2Pgs::Encode(Stream ^rgbaStream, IFrameStreamAdvisor ^advisor, Stream ^output, IProgressReporter ^reporter)
	{
		ClrStreamEx tInput{ AssertClrArgumentNotNull(rgbaStream, "rgbaStream") };
		std::unique_ptr<FrameStreamAdvisor> tAdvisor{ (advisor == nullptr) ? nullptr : new ClrAdvisor(advisor) };
		BgraRawStream frameStream{ &tInput, tAdvisor.get() };

		ClrStreamEx tOutput{ AssertClrArgumentNotNull(output, "output") };
		std::unique_ptr<ClrReporter> tReporter{ (reporter == nullptr) ? nullptr : new ClrReporter(reporter) };

		encoderNative->Encode(&frameStream, &tOutput, tReporter.get());
	}

	void Avs2Pgs::Encode(FileInfo ^avsFile, IFrameStreamAdvisor ^advisor, Stream ^output, IProgressReporter ^reporter)
	{
		pin_ptr<const TCHAR> tPinedFileName = PtrToStringChars(avsFile->FullName);
		TCHAR* tInput = const_cast<TCHAR*>(tPinedFileName);

		std::unique_ptr<FrameStreamAdvisor> tAdvisor{ (advisor == nullptr) ? nullptr : new ClrAdvisor(advisor) };
		BgraAvsStream frameStream{ tInput, tAdvisor.get() };

		ClrStreamEx tOutput{ AssertClrArgumentNotNull(output, "output") };
		std::unique_ptr<ClrReporter> tReporter{ (reporter == nullptr) ? nullptr : new ClrReporter(reporter) };

		encoderNative->Encode(&frameStream, &tOutput, tReporter.get());
	}

	Avs2Pgs::!Avs2Pgs()
	{
		if (encoderNative != nullptr)
		{
			delete encoderNative;
			encoderNative = nullptr;
		}
		if (settingsNative != nullptr)
		{
			delete settingsNative;
			settingsNative = nullptr;
		}
		if (loggerNative != nullptr)
		{
			delete loggerNative;
			loggerNative = nullptr;
		}
	}

	Avs2Pgs::~Avs2Pgs()
	{
		this->!Avs2Pgs();
		this->disposed = true;
	}

}