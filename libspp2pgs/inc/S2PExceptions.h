/*----------------------------------------------------------------------------
* avs2pgs - Generates BluRay PG Stream from RGBA AviSynth scripts
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

#include <tchar.h>
#include "ExceptionFormatters.h"

namespace avs2pgs
{
	enum struct A2PExceptionType
	{
		Unknown,
		Application,
		NotImplenmented,
		ArgumentNull,

		AvsInitFailed,
		TempOutputFailed,
		ImageOperationFailed,
		EpochManagingFailed,
		StreamOperationFailed,
		GxBufferManagingFailed,
	};
	
	class A2PException
	{
	public:
		A2PException(A2PExceptionType type, const ExceptionFormatter * messageFormatter)
			:type(type), messageFormatter(messageFormatter){}

		virtual ~A2PException() {};

		inline A2PExceptionType GetType() const { return type; }
		inline int FormatMessage(TCHAR *buffer, int bufferSize)
		{
			if (messageFormatter) { return messageFormatter->Invoke(buffer, bufferSize); }
			else { buffer[0] = _T('\0'); return 1; }
		}

		static int const MaxBufferSize = 4096;

	private:
		A2PExceptionType type;
		const ExceptionFormatter * const messageFormatter;
	};

	class EndUserException : public A2PException
	{
	public:
		EndUserException(const TCHAR * const message)
			:formatter(message), A2PException(A2PExceptionType::Application, &formatter) {}

		~EndUserException() {};

	private:
		StaticExceptionFormatter formatter;
	};

	enum struct AvsInitExceptionType
	{
		Unknown,

		AvsStreamOpenFailed,
		AvsGetInfoFailed,
		AvsFormatIncorrect,
	};

	class AvsInitException : public A2PException
	{
	public:
		AvsInitException(AvsInitExceptionType type, HRESULT error)
			:A2PException(A2PExceptionType::AvsInitFailed, nullptr), type(type), error(error) {}

		~AvsInitException() {};

		inline AvsInitExceptionType GetType() const { return type; }

	private:
		AvsInitExceptionType type;
		HRESULT error;
	};


	enum struct TempOutputExceptionType
	{
		Unknown,

		FetchTempOutputPathFailed,
		QueryFreeSpaceFailed,
		NotEnoughFreeSpace,
		FetchTempFileNameFailed,
		TempFileCreateFailed,

		WritingFailed,
		ReadingFailed
	};


	class TempOutputException : public A2PException
	{
	public:
		TempOutputException(TempOutputExceptionType type, DWORD error)
			:error(error), exFormatter(&(this->error)), A2PException(A2PExceptionType::TempOutputFailed, &(this->exFormatter)), type(type) {}

		~TempOutputException() {};

		inline TempOutputExceptionType GetType() const { return type; }

	private:
		DWORD error;
		SystemExceptionFormatter exFormatter;
		TempOutputExceptionType type;
	};



	enum struct ImageOperationExceptionType
	{
		Unknown,

		InvalidImageSize,
		InvalidImagePixelFormat,

		PaletteSizeOverflow,
	};


	class ImageOperationException : public A2PException
	{
	public:
		ImageOperationException(ImageOperationExceptionType type)
			:A2PException(A2PExceptionType::ImageOperationFailed, nullptr), type(type) {}

		~ImageOperationException() {};

		inline ImageOperationExceptionType GetType() const { return type; }

	private:
		ImageOperationExceptionType type;
	};


	enum struct EpochManagingExceptionType
	{
		Unknown,
		DisplaySetEnqueueWhileDequeuing,
	};


	class EpochManagingException : public A2PException
	{
	public:
		EpochManagingException(EpochManagingExceptionType type)
			:A2PException(A2PExceptionType::EpochManagingFailed, nullptr), type(type) {}

		~EpochManagingException() {};

		inline EpochManagingExceptionType GetType() const { return type; }

	private:
		EpochManagingExceptionType type;
	};




	enum struct StreamOperationExceptionType
	{
		Unknown,
		InvalidOperation,
		OperationFailed,
	};


	class StreamOperationException : public A2PException
	{
	public:
		StreamOperationException(StreamOperationExceptionType type)
			:A2PException(A2PExceptionType::StreamOperationFailed, nullptr), type(type) {}

		~StreamOperationException() {};

		inline StreamOperationExceptionType GetType() const { return type; }

	private:
		StreamOperationExceptionType type;
	};


	enum struct GxBufferExceptionType
	{
		Unknown,
		InvalidOperation,
		OperationFailed,

		ResourceExhausted,

	};


	class GxBufferException : public A2PException
	{
	public:
		GxBufferException(GxBufferExceptionType type)
			:A2PException(A2PExceptionType::GxBufferManagingFailed, nullptr), type(type) {}

		~GxBufferException() {};

		inline GxBufferExceptionType GetType() const { return type; }

	private:
		GxBufferExceptionType type;
	};

}