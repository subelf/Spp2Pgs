
#pragma once

#include "FrameStream.h"
#include "StreamEx.h"
#include "ProgressReporter.h"

namespace avs2pgs
{

	int Avs2Pgs(FrameStream *input, StreamEx *output, ProgressReporter *reporter);

}