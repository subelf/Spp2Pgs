#pragma once

#include "PgsEncoder.h"

namespace Spp2PgsNet
{
	using namespace spp2pgs;
	
	public ref class PgsEncoderNet
	{
	public:
		PgsEncoderNet::!PgsEncoderNet()
		{
			if (nativeEncoder != nullptr)
			{
				delete nativeEncoder;
				nativeEncoder = nullptr;
			}
			if (outputStream != nullptr)
			{
				delete outputStream;
				outputStream = nullptr;
			}
		}

		PgsEncoderNet::~PgsEncoderNet()
		{
			this->!PgsEncoderNet();
			this->isDisposed = true;
		}

	internal:
		PgsEncoderNet(PgsEncoder *pgsEncoder, StreamEx *output) :
			nativeEncoder(AssertClrArgumentNotNull(pgsEncoder, "pgsEncoder")), 
			outputStream(AssertClrArgumentNotNull(output, "output")) {}

		property PgsEncoder * NativeEncoder
		{
			PgsEncoder* get()
			{
				AssertNotDisposed();
				return nativeEncoder;
			};
		};

	private:
		PgsEncoder *nativeEncoder = nullptr;
		StreamEx *outputStream = nullptr;

		bool isDisposed = false;

		void AssertNotDisposed()
		{
			String ^tObjName = "PgsEncoderNet";
			if (this->isDisposed) throw gcnew ObjectDisposedException(tObjName);
		}
	};
}