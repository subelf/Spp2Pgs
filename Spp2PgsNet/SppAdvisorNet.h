#pragma once

#include <FrameStreamAdvisor.h>

#include "IFrameStreamAdvisor.h"

namespace Spp2PgsNet
{
	using namespace spp2pgs;

	public ref class SppAdvisorNet :
		public IFrameStreamAdvisor
	{
	public:
		virtual int IsBlank(int index)
		{
			AssertNotDisposed();
			return nativeSppAdvisor->IsBlank(index);
		}
		virtual int IsIdentical(int index1, int index2)
		{
			AssertNotDisposed();
			return nativeSppAdvisor->IsIdentical(index1, index2);
		}

		virtual property int FirstPossibleImage
		{
			int get() {
				AssertNotDisposed();
				return nativeSppAdvisor->GetFirstPossibleImage();
			};
		}
	
		virtual property int LastPossibleImage
		{
			int get() {
				AssertNotDisposed();
				return nativeSppAdvisor->GetLastPossibleImage();
			};
		}

		virtual property int FrameIndexOffset
		{
			int get() {
				AssertNotDisposed();
				return nativeSppAdvisor->GetFrameIndexOffset();
			};
		}

		virtual property BluraySharp::Common::BdViFormat FrameFormat
		{
			BluraySharp::Common::BdViFormat get() {
				AssertNotDisposed();
				return static_cast<BluraySharp::Common::BdViFormat>(nativeSppAdvisor->GetFrameFormat());
			};
		}

		virtual property BluraySharp::Common::BdViFrameRate FrameRate
		{
			BluraySharp::Common::BdViFrameRate get() {
				AssertNotDisposed();
				return static_cast<BluraySharp::Common::BdViFrameRate>(nativeSppAdvisor->GetFrameRate());
			};
		}

		!SppAdvisorNet()
		{
			if (this->nativeSppAdvisor != nullptr)
			{
				delete nativeSppAdvisor;
				this->nativeSppAdvisor = nullptr;
			}
		}

		~SppAdvisorNet()
		{
			this->!SppAdvisorNet();
			isDisposed = true;
		}

	internal:
		SppAdvisorNet(FrameStreamAdvisor *sppAdvisor) :
			nativeSppAdvisor(AssertClrArgumentNotNull(sppAdvisor, "sppAdvisor"))
		{}

	private:
		FrameStreamAdvisor *nativeSppAdvisor = nullptr;

		bool isDisposed = false;

		void AssertNotDisposed()
		{
			String ^tObjName = "SppAdvisorNet";
			if (this->isDisposed) throw gcnew ObjectDisposedException(tObjName);
		}
	};

}