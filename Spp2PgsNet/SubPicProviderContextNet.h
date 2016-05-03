#pragma once

#include <VSSppfApi.h>

#include "Global.h"

namespace Spp2PgsNet
{
	using namespace spp2pgs;

	public ref class SubPicProviderContextNet
	{
	public:
		void Lock() { AssertNotDisposed(); this->nativeCtx->Lock(); }
		void Unlock() { AssertNotDisposed(); this->nativeCtx->Unlock(); }

		!SubPicProviderContextNet()
		{
			if (this->nativeCtx != nullptr)
			{
				this->nativeCtx->Release();
				this->nativeCtx = nullptr;
			}
		}

		~SubPicProviderContextNet()
		{
			this->!SubPicProviderContextNet();
			isDisposed = true;
		}

	internal:
		SubPicProviderContextNet(IVobSubPicProviderContext *sppCtx):
			nativeCtx(AssertClrArgumentNotNull(sppCtx, "sppCtx"))
		{
			sppCtx->AddRef();
		}

		property IVobSubPicProviderContext *NativeSppContext
		{
			IVobSubPicProviderContext* get()
			{ 
				AssertNotDisposed();
				return this->nativeCtx; 
			};
		}

	private:
		IVobSubPicProviderContext *nativeCtx = nullptr;

		bool isDisposed = false;

		void AssertNotDisposed()
		{
			String ^tObjName = "SubPicProviderContextNet";
			if (this->isDisposed) throw gcnew ObjectDisposedException(tObjName);
		}
	};

}