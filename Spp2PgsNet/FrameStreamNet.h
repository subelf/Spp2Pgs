#pragma once

#include <FrameStream.h>
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
		}

		FrameStreamNet::~FrameStreamNet()
		{
			advisor = nullptr;

			this->!FrameStreamNet();

			this->isDisposed = true;
		}

	internal:
		FrameStreamNet(FrameStream *frameStream, IFrameStreamAdvisor ^advisor) :
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
		IFrameStreamAdvisor ^advisor = nullptr;

		bool isDisposed = false;

		void AssertNotDisposed()
		{
			String ^tObjName = "FrameStreamNet";
			if (this->isDisposed) throw gcnew ObjectDisposedException(tObjName);
		}
	};
}


