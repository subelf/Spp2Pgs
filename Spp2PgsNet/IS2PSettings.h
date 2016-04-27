#pragma once

namespace Avs2PgsNet
{
	using namespace System;

	public interface class IA2PSettings
	{
		property UInt64 MaxCachingSize { UInt64 get(); };
		property int MaxImageBlockSize { int get(); };
		property bool IsForcingEpochZeroStart { bool get(); }
		property bool IsForcingTmtCompat { bool get(); }
	};

}