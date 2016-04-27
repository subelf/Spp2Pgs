#pragma once

namespace Spp2PgsNet
{
	using namespace System;

	public interface class IS2PLogger
	{
		void Vlog(int level, String^ msg);
	};

}