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

#pragma warning( disable : 4290 )

#include <memory>

#include <tchar.h>
#include <Windows.h>

#include "libavs2pgs.h"

#include <CfileStreamEx.h>
#include <BgraAvsStream.h>
#include <BgraFrame.h>

using namespace avs2pgs;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
		return 1;

	BgraAvsStream avs{ argv[1], nullptr };
	BgraFrame frame{ avs.GetFrameSize() };

	CfileStreamEx output{ stdout, false, true, false, nullptr };

	int ind;
	do
	{
		 ind = frame.ReadNextOf(&avs);
		 unsigned char* buf = (unsigned char*)(frame.GetDataBuffer());
		 auto const &size = frame.GetDataSize();

		 output.Write(buf, 0, size);
	} 
	while (ind != -1);

	return 0;
}