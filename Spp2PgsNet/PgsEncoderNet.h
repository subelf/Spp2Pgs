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

#include "PgsEncoder.h"

namespace Spp2PgsNet
{
	using namespace spp2pgs;
	
	public ref class PgsEncoderNet sealed
	{
	public:
		void RegistAnchor(int frameOffset)
		{
			this->nativeEncoder->RegistAnchor(frameOffset);
		}

		void FlushAnchor()
		{
			this->nativeEncoder->FlushAnchor();
		}

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