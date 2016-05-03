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