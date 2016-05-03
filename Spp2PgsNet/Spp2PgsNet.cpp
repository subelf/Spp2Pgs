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

#include "stdafx.h"

#include <memory>
#include <vcclr.h>

#include <SppAdvisor.h>
#include <BgraRawStream.h>
#include <BgraAvsStream.h>
#include <BgraSubPicStream.h>

#include "ClrStreamEx.h"
#include "ClrAdvisor.h"
#include "ClrReporter.h"
#include "Spp2PgsNet.h"
#include "oledll.h"
#include "SppAdvisorNet.h"

#ifdef DEBUG
#define SPPF_DLL_NAME_EXT L"D.dll"
#else
#define SPPF_DLL_NAME_EXT L".dll"
#endif
#ifdef _WIN64
#define SPPF_DLL_NAME_PRE L"xy-VSsppf64"
#else
#define SPPF_DLL_NAME_PRE L"xy-VSsppf32"
#endif
#define SPPF_DLL_NAME (SPPF_DLL_NAME_PRE SPPF_DLL_NAME_EXT)

namespace Spp2PgsNet
{
	using namespace System::Runtime::InteropServices;

	void Spp2Pgs::InitS2PEncoder()
	{
		bool isOk = true;
		std::auto_ptr<ClrSettings> tSettings(new ClrSettings{ settings });
		std::auto_ptr<ClrLogger> tLogger;
		std::auto_ptr<S2PEncoder> tS2P;

		if (tSettings.get() == nullptr)
		{
			isOk = false;
		}

		if (isOk)
		{
			tLogger.reset(new ClrLogger{ logger });
			if (tLogger.get() == nullptr)
			{
				isOk = false;
			}
		}

		if (isOk)
		{
			tS2P.reset(new spp2pgs::S2PEncoder{ tSettings.get(), tLogger.get() });
			if (tS2P.get() == nullptr)
			{
				isOk = false;
			}
		}

		if (isOk)
		{
			this->loggerNative = tLogger.release();
			this->settingsNative = tSettings.release();

			this->encoderNative = tS2P.release();
		}
		else
		{
			throw gcnew OutOfMemoryException("Spp2PgsNet Initializing Failed!");
		}
	}

	void Spp2Pgs::InitSppf()
	{
		CComPtr<IVobSubPicProviderAlfaFactory> ifpSppf;
		auto hr = CoCreateInstanceFormDllFile(
			CLSID_VobSubPicProviderAlfaFactory,
			IID_VobSubPicProviderAlfaFactory,
			(LPVOID*)&ifpSppf,
			SPPF_DLL_NAME);

		if (SUCCEEDED(hr))
		{
			sppf = ifpSppf.Detach();
		}
		else
		{
			throw gcnew COMException();
		}
	}

	Spp2Pgs::Spp2Pgs(IS2PSettings ^ settings, IS2PLogger ^ logger)
		: settings(AssertClrArgumentNotNull(settings, "settings")), logger(AssertClrArgumentNotNull(logger, "logger"))
	{
		this->InitS2PEncoder();
		this->InitSppf();
	}

	PgsEncoderNet^ Spp2Pgs::CreatePgsEncoder(Stream ^ output, BluraySharp::Common::BdViFormat format, BluraySharp::Common::BdViFrameRate rate, int syncFrameOffset)
	{
		AssertNotDisposed();

		std::auto_ptr<StreamEx> tOutputStream(new ClrStreamEx(output));

		auto const &tFormat = static_cast<spp2pgs::BdViFormat>(format);
		auto const &tRate = static_cast<spp2pgs::BdViFrameRate>(rate);
		spp2pgs::Size const &tSize = spp2pgs::GetFrameSize(tFormat);

		std::auto_ptr<PgsEncoder> tPgsEncoder(new PgsEncoder(encoderNative, tOutputStream.get(), tSize, tRate, syncFrameOffset));

		return gcnew PgsEncoderNet(tPgsEncoder.release(), tOutputStream.release());
	}
	/*
	FrameStreamNet^ Spp2Pgs::CreateFrameStreamFromSubtitleFile(FileInfo ^ subFile)
	{

		if (SUCCEEDED(hr))
		{
			hr = pSppf->CreateContext(&pContext);
		}

		if (SUCCEEDED(hr))
		{
			hr = pSppf->CreateProvider(pContext, input, &pSpp);
		}

		if (SUCCEEDED(hr))
		{
			sppAdvisor.reset(new SppAdvisor(pSpp, format, rate, begin, end));
			avstream.reset(new BgraSubPicStream(pSpp, sppAdvisor.get()));
		}
	}
	*/

	/*
	void Spp2Pgs::Encode(Stream ^rgbaStream, IFrameStreamAdvisor ^advisor, Stream ^output, IProgressReporter ^reporter)
	{
		ClrStreamEx tInput{ AssertClrArgumentNotNull(rgbaStream, "rgbaStream") };
		std::unique_ptr<FrameStreamAdvisor> tAdvisor{ (advisor == nullptr) ? nullptr : new ClrAdvisor(advisor) };
		BgraRawStream frameStream{ &tInput, tAdvisor.get() };

		ClrStreamEx tOutput{ AssertClrArgumentNotNull(output, "output") };
		std::unique_ptr<ClrReporter> tReporter{ (reporter == nullptr) ? nullptr : new ClrReporter(reporter) };

		encoderNative->Encode(&frameStream, &tOutput, tReporter.get());
	}

	void Spp2Pgs::Encode(FileInfo ^avsFile, IFrameStreamAdvisor ^advisor, Stream ^output, IProgressReporter ^reporter)
	{
		pin_ptr<const TCHAR> tPinedFileName = PtrToStringChars(avsFile->FullName);
		TCHAR* tInput = const_cast<TCHAR*>(tPinedFileName);

		std::unique_ptr<FrameStreamAdvisor> tAdvisor{ (advisor == nullptr) ? nullptr : new ClrAdvisor(advisor) };
		BgraAvsStream frameStream{ tInput, tAdvisor.get() };

		ClrStreamEx tOutput{ AssertClrArgumentNotNull(output, "output") };
		std::unique_ptr<ClrReporter> tReporter{ (reporter == nullptr) ? nullptr : new ClrReporter(reporter) };

		encoderNative->Encode(&frameStream, &tOutput, tReporter.get());
	}
	*/

	FrameStreamNet ^ Spp2Pgs::CreateFrameStream(SubPicProviderNet ^ spp, IFrameStreamAdvisor ^ advisor)
	{
		AssertNotDisposed();

		CComPtr<ISubPicProviderAlfa> ifpSpp = spp->NativeProvider;
		std::auto_ptr<ClrAdvisor> apAdvisor(new ClrAdvisor(advisor));
		std::auto_ptr<BgraSubPicStream> apSppStream;

		if (apAdvisor.get() != nullptr)
		{
			apSppStream.reset(new BgraSubPicStream(ifpSpp, apAdvisor.release()));
		}
		else
		{
			throw gcnew OutOfMemoryException("CreateClrAdvisor@CreateFrameStream Failed!");
		}

		if (apSppStream.get() != nullptr)
		{
			return gcnew FrameStreamNet(apSppStream.release(), advisor);
		}
		else
		{
			throw gcnew OutOfMemoryException("CreateFrameStream Failed!");
		}
	}

	SubPicProviderContextNet ^ Spp2Pgs::CreateSubPicProviderContext()
	{
		AssertNotDisposed();

		CComPtr<IVobSubPicProviderContext> ifpSppCtx;
		HRESULT hr = this->sppf->CreateContext(&ifpSppCtx);

		if (SUCCEEDED(hr))
		{
			return gcnew SubPicProviderContextNet(ifpSppCtx);
		}
		else
		{
			throw gcnew COMException("CreateSubPicProviderContext Failed!", hr);
		}
	}

	SubPicProviderNet ^ Spp2Pgs::CreateSubPicProvider(SubPicProviderContextNet ^ context, FileInfo ^subtitleFile)
	{
		AssertNotDisposed();

		CComPtr<IVobSubPicProviderContext> ifpSppCtx = context->NativeSppContext;
		CComPtr<ISubPicProviderAlfa> ifpSpp;
		
		pin_ptr<const TCHAR> tPinedFileName = PtrToStringChars(subtitleFile->FullName);
		TCHAR* tFileName = const_cast<TCHAR*>(tPinedFileName);

		HRESULT hr = this->sppf->CreateProvider(ifpSppCtx, tFileName, &ifpSpp);

		if (SUCCEEDED(hr))
		{
			return gcnew SubPicProviderNet(ifpSpp, context);
		}
		else
		{
			throw gcnew COMException("CreateSubPicProvider Failed!", hr);
		}
	}

	IFrameStreamAdvisor ^ Spp2Pgs::CreateSppFrameStreamAdvisor(SubPicProviderNet ^spp, BluraySharp::Common::BdViFormat format, BluraySharp::Common::BdViFrameRate rate, int from, int to, int offset)
	{
		AssertNotDisposed();

		CComPtr<ISubPicProviderAlfa> ifpSpp = spp->NativeProvider;

		auto tFormat = static_cast<spp2pgs::BdViFormat>(format);
		auto tRate = static_cast<spp2pgs::BdViFrameRate>(rate);

		std::auto_ptr<SppAdvisor> apAdvisor(new SppAdvisor(ifpSpp, tFormat, tRate, from, to, offset));

		if (apAdvisor.get() != nullptr)
		{
			return gcnew SppAdvisorNet(apAdvisor.release());
		}
		else
		{
			throw gcnew OutOfMemoryException("CreateSppFrameStreamAdvisor Failed!");
		}
	}

	bool Spp2Pgs::Encode(FrameStreamNet ^ input, PgsEncoderNet ^ output, IProgressReporter ^ reporter)
	{
		FrameStream *frameStream = input->NativeStream;
		PgsEncoder *pgsEnc = output->NativeEncoder;

		std::unique_ptr<ClrReporter> tReporter{
			(reporter == nullptr) ? nullptr : new ClrReporter(reporter)
		};

		return this->encoderNative->Encode(frameStream, pgsEnc, tReporter.get());
	}

	Spp2Pgs::!Spp2Pgs()
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

		if (sppf != nullptr)
		{
			sppf->Release();
			sppf = nullptr;
		}
	}

	Spp2Pgs::~Spp2Pgs()
	{
		this->!Spp2Pgs();

		settings = nullptr;
		logger = nullptr;

		this->isDisposed = true;
	}


}