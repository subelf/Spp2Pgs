#pragma once

namespace Avs2PgsNet
{
	using namespace System;

	public interface class IA2PLogger
	{
		void Vlog(int level, String^ msg);
	};

}