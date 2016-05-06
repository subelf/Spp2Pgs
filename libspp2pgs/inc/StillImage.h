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

#include "GraphicalTypes.h"

#include <vector>

namespace spp2pgs
{

	enum AlignMask
	{
		Align4 = 0x03,
		Align8 = 0x07,
		Align16 = 0x0f,
		Align32 = 0x1f,
		Align64 = 0x3f,
	};

	class StillImage
	{
	public:
		//stride: 0 - for auto;
		StillImage(Size imageSize, int pixelSize, int stride = 0);
		virtual ~StillImage();

	public:
		inline char *GetDataBuffer() const { return this->dataBuffer; }
		inline int GetWidth() const { return this->imageSize.w; }
		inline int GetStride() const{ return this->stride; }
		inline int GetHeight() const{ return this->imageSize.h; }
		inline Size GetImageSize() const { return this->imageSize; }
		inline int GetDataSize() const { return this->dataSize; }
		inline int GetPixelSize() const { return this->pixelSize; }

		void AnnounceBlank() { this->isBlank = 1; }
		void AnnounceDirty() { this->isBlank = 0; this->isExplicitErased = false; }
		void AnnounceModified() { this->isBlank = -1; this->isExplicitErased = false; }

		void AnnounceNonNormalized() { this->isNormalized = false; }
		void AnnounceNormalized() { this->isNormalized = true; }

		void Normalize();	//erease all pixels with alpha=0;
		bool IsIdenticalTo(StillImage *image);
		inline bool IsBlank() { return (this->isBlank = static_cast<StillImage const *>(this)->IsBlank() ? 1 : 0) != 0; }
		bool IsBlank() const;

		void Erase();
		void Erase(Rect const& rect) { this->ExplicitErase(rect); };

		void RegisterRrawnRects(Rect const& rect) { 
			AnnounceModified();
			this->drawnRects.push_back(rect);
		}

	protected:
		virtual bool IsExplicitIdenticalTo(StillImage const *image) const = 0;
		virtual bool IsExplicitBlank() const = 0;
		virtual void ExplicitEraseTransparents() const = 0;
		virtual void ExplicitErase(Rect rect) const = 0;

	private:
		Size imageSize;
		int stride;
		int pixelSize;

		char* buffer;
		char* dataBuffer;
		int dataSize;

		int isBlank = -1;
		bool isNormalized = false;
		bool isExplicitErased = false;

		std::vector<Rect> drawnRects;

		template<typename T> static inline T* AlignTo(T *input, AlignMask mask)
		{
			return reinterpret_cast<T*>((reinterpret_cast<uintptr_t>(input) + mask) & ~mask);
		}

		template<typename T> static inline T AlignTo(T input, AlignMask mask)
		{
			return ((input + mask) & ~mask);
		}

		void InvalidateRrawnRects() { this->drawnRects.clear(); }

	};

}