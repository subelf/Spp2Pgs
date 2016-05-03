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

#include <VSSppfApi.h>


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