#pragma once

#include <A2PExceptions.h>

namespace Avs2PgsNet 
{
	using namespace System;

	template<typename T> inline T^ AssertClrArgumentNotNull(T^ argument)
	{
		if (argument == nullptr)
		{
			throw avs2pgs::A2PException(avs2pgs::A2PExceptionType::ArgumentNull, nullptr);
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