#pragma once

#include <S2PExceptions.h>

namespace Spp2PgsNet 
{
	using namespace System;

	template<typename T> inline T^ AssertClrArgumentNotNull(T^ argument)
	{
		if (argument == nullptr)
		{
			throw spp2pgs::S2PException(spp2pgs::S2PExceptionType::ArgumentNull, nullptr);
		}

		return argument;
	}

	template<typename T> inline T^ AssertClrArgumentNotNull(T^ argument, String ^name)
	{
		if (argument == nullptr)
		{
			throw gcnew ArgumentNullException(name);
		}

		return argument;
	}

}