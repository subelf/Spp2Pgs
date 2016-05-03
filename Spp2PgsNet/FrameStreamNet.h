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

#include <FrameStream.h>
#include <StreamEx.h>
#include "IFrameStreamAdvisor.h"

namespace Spp2PgsNet
{
	using namespace spp2pgs;
	using namespace System::IO;

	public ref class FrameStreamNet
	{
	public:
		FrameStreamNet::!FrameStreamNet()
		{
			if (nativeStream != nullptr)
			{
				delete nativeStream;
				nativeStream = nullptr;
			}

			if (advisor != nullptr)
			{
				delete advisor;
				advisor = nullptr;
			}
		}

		virtual FrameStreamNet::~FrameStreamNet()
		{
			this->!FrameStreamNet();

			this->isDisposed = true;
		}

	internal:
		FrameStreamNet(FrameStream *frameStream, FrameStreamAdvisor *advisor) :
			nativeStream(AssertClrArgumentNotNull(frameStream, "frameStream")),
			advisor(AssertClrArgumentNotNull(advisor, "advisor")) {}
		
		property FrameStream * NativeStream
		{
			FrameStream* get()
			{
				AssertNotDisposed();
				return nativeStream;
			};
		};

	private:
		FrameStream *nativeStream = nullptr;
		FrameStreamAdvisor *advisor = nullptr;

		bool isDisposed = false;

		void AssertNotDisposed()
		{
			String ^tObjName = "FrameStreamNet";
			if (this->isDisposed) throw gcnew ObjectDisposedException(tObjName);
		}
	};

	private ref class RawFrameStreamNet :
		public FrameStreamNet
	{
	public:
		RawFrameStreamNet(FrameStream *frameStream, FrameStreamAdvisor *advisor, StreamEx *rawStream) :
			FrameStreamNet(frameStream, advisor),
			rawStream(AssertClrArgumentNotNull(rawStream, "rawStream")) {}

		RawFrameStreamNet::!RawFrameStreamNet()
		{
			if (rawStream != nullptr)
			{
				delete rawStream;
				rawStream = nullptr;
			}
		}

		virtual RawFrameStreamNet::~RawFrameStreamNet() override
		{
			this->!RawFrameStreamNet();

			this->isDisposed = true;
		}

	private:
		StreamEx *rawStream;
		bool isDisposed = false;

		void AssertNotDisposed()
		{
			String ^tObjName = "RawFrameStreamNet";
			if (this->isDisposed) throw gcnew ObjectDisposedException(tObjName);
		}
	};

}


