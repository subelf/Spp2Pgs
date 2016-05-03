#pragma once

#include "SubPicProviderContextNet.h"

namespace Spp2PgsNet
{
	using namespace spp2pgs;
	
	public ref class SubPicProviderNet
	{
	public:
		SubPicProviderNet::!SubPicProviderNet()
		{
			if (nativeSpp != nullptr)
			{
				nativeSpp->Release();
				nativeSpp = nullptr;
			}
		}

		SubPicProviderNet::~SubPicProviderNet()
		{
			this->!SubPicProviderNet();
		
			if (this->sppCtx != nullptr)
			{
				this->sppCtx->~SubPicProviderContextNet();
				this->sppCtx = nullptr;
			}

			this->isDisposed = true;
		}

	internal:
		SubPicProviderNet(ISubPicProviderAlfa *spp, SubPicProviderContextNet ^sppCtx) :
			nativeSpp(spp), sppCtx(sppCtx) 
		{
			this->nativeSpp->AddRef();
		}

		property ISubPicProviderAlfa * NativeProvider
		{
			ISubPicProviderAlfa* get()
			{
				AssertNotDisposed();
				return nativeSpp;
			};
		};

	private:
		ISubPicProviderAlfa *nativeSpp;

		SubPicProviderContextNet ^sppCtx;

		bool isDisposed = false;

		void AssertNotDisposed()
		{
			String ^tObjName = "SubPicProviderNet";
			if (this->isDisposed) throw gcnew ObjectDisposedException(tObjName);
		}
	};

}