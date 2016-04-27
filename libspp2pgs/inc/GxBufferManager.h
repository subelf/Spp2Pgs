/*----------------------------------------------------------------------------
* spp2pgs - Generates BluRay PG Stream from RGBA AviSynth scripts
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

#include "Global.h"
#include "S2PControllerBase.h"

namespace spp2pgs
{
	int const CompositionCountMax = 8;
	int const ObjectCountMax = 64;
	int const EncodeBufferSize = ((((FrameWidthMax * FrameHeightMax + PaletteItemsMax * 5) >> 20) + 1) << 20);

	int const CompositionObjectCountMax = 2;
	int const WindowCountMax = 2;

	struct HashCode{
		bool operator == (HashCode& hash) { return false; }
		bool operator != (HashCode& hash) { return true; }
	};

	template<typename TBuf> class GxBuffer final
	{
	public:
		GxBuffer()
			: GxBuffer(-1) {}
		GxBuffer(int id)
			: id(id), refCount(0), version(-1) {}
		~GxBuffer() {}

		inline int GetId() const { return id; }

		inline void Clear() { version = -1; refCount = 0; memset(&buffer, 0, sizeof(buffer)); }

		inline int GetVersion() const { return version; }
		inline bool IsUsed() const { return version >= 0; }
		inline void UpdateVersion() { if (refCount) throw GxBufferException(GxBufferExceptionType::InvalidOperation); version++; }

		inline bool IsInUse() const { return refCount > 0; }
		inline void AddRef() { refCount++; }
		inline void Release() { refCount = refCount ? (refCount - 1) : 0; }

		inline operator TBuf * () { return &buffer; }
		inline TBuf * operator ->() { return &buffer; }

		inline operator TBuf const * () const { return &buffer; }
		inline TBuf const * operator ->() const { return &buffer; }

	private:
		int id;
		int version;
		int refCount;
		TBuf buffer;
	};

	struct PaletteBuffer final
	{
		int hashLength;
		HashCode hash;
	};

	struct ObjectBuffer final
	{
		Size dataSize;

		Rect hashRect;
		HashCode imageHash;
	};

	struct WindowsDescriptor final
	{
		int count;
		Rect windows[WindowCountMax];

		inline int EstimateDecodeDuration(int id) const { return GetWindowDecodeDuration(windows[id].Area()); }
		inline int EstimateDecodeDuration() const {
			int tArea = 0;
			for (int i = 0; i < count; tArea += windows[i++].Area());
			return GetWindowDecodeDuration(tArea);
		}
	};

	struct CompositionObjectDescriptor final
	{
		int windowRef;
		Position objectPos;
		Rect imageCrop;

		GxBuffer<ObjectBuffer> const * objectRef;
		int encodedObjectLength;
		unsigned __int8 * encodedObject;

		inline int EstimateDecodeDuration() const {
			auto const &tMaxSize = Size{ FrameWidthMax, FrameHeightMax };
			auto const &tEstSize = ((objectRef == nullptr) ? tMaxSize : (*objectRef)->dataSize);
			return GetObjectDecodeDuration(tEstSize.Area(), false);
		}
	};

	struct CompositionBuffer
	{
		__int64 pts, ets;
		WindowsDescriptor const * windows;

		GxBuffer<PaletteBuffer> const * paletteRef;
		int encodedPaletteLength;
		unsigned __int8 * encodedPalette;

		int cmpnObjsCount;
		CompositionObjectDescriptor cmpnObjs[CompositionObjectCountMax];

		int EstimateDecodeDuration(int initDuration = 1) const;
	};

	class GxBufferManager final
		: public S2PControllerBase
	{
	public:
		GxBufferManager(S2PContext const *context);
		~GxBufferManager();

		void ReleaseEncodeBuffer() { encodeBufferAllocPointer = 0; }
		unsigned __int8 * AllocateEncodeBuffer(int size);

		//release all compositions whose pts is above dts;
		void UpdateTime(__int64 dts);
		void Clear();
		inline void SetObjectDataSizeLimit(Size const& size) { this->maxDataSize = size; }

		void ReleaseCompositionBuffer(CompositionBuffer * buffer);

		GxBuffer<CompositionBuffer> * FindFreeCompositionBuffer();
		GxBuffer<PaletteBuffer> * FindFreePaletteBuffer();
		//find object buffer larger than dataCrop and not beyond the window
		GxBuffer<ObjectBuffer> * FindProperObjectBuffer(Size min, Size max);

		GxBuffer<PaletteBuffer> * FindMatchedPaletteBuffer(int size, HashCode hash);
		GxBuffer<ObjectBuffer> * FindMatchedObjectBuffer(Size size, HashCode hash);

	private:
		static int const ObjectBufferSizeMax = 4 * 1024 * 1024;

		//encode buffer
		int encodeBufferAllocPointer;
		unsigned __int8 encodeBuffer[EncodeBufferSize];
		
		Size maxDataSize;
		int objectBufferSize;

		//__int64 dts;
		GxBuffer<PaletteBuffer> paletteBuffers[CompositionCountMax];
		GxBuffer<ObjectBuffer> objectBuffers[ObjectCountMax];
		GxBuffer<CompositionBuffer> compositionBuffers[CompositionCountMax];
	};

}