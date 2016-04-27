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

#include <A2PEncoder.h>

#include <CfileStreamEx.h>
#include <BgraAvsStream.h>
#include <BgraRawStream.h>

#include "getopt.h"
#include "A2PStdLogger.h"
#include "A2PClaSettings.h"
#include "ClaAdvisor.h"

using namespace avs2pgs;

BdViFormat GetFormat(int optValue)
{
	switch (optValue)
	{
	case 1:
	case 240:
	case -480:
		return BdViFormat::Vi480i;
	case 2:
	case 288:
	case -576:
		return BdViFormat::Vi576i;
	case 3:
	case 480:
		return BdViFormat::Vi480p;
	case 4:
	case 540:
	case -1080:
		return BdViFormat::Vi1080i;
	case 5:
	case 720:
		return BdViFormat::Vi720p;
	case 6:
	case 1080:
		return BdViFormat::Vi1080p;
	case 7:
	case 576:
		return BdViFormat::Vi576p;
	default:
		return BdViFormat::Unknown;
	}
}

BdViFrameRate GetRate(int optValue)
{
	switch (optValue)
	{
	case 1:
	case 23:
		return BdViFrameRate::Vi23;
	case 2:
	case 24:
		return BdViFrameRate::Vi24;
	case 3:
	case 25:
		return BdViFrameRate::Vi25;
	case 4:
	case 29:
		return BdViFrameRate::Vi29;
	case 5:
	case 30:
		return BdViFrameRate::Vi30;
	case 6:
	case 50:
		return BdViFrameRate::Vi50;
	case 7:
	case 59:
		return BdViFrameRate::Vi59;
	case 8:
	case 60:
		return BdViFrameRate::Vi60;
	default:
		return BdViFrameRate::Unknown;
	}
}

void print_usage(A2PLogger const *logger)
{
	logger->Log(A2PLogger::error, _T(
		"AVS2PGS Convert a bgra avstream to bluray PG stream.\n\n"
		"USAGE: Avs2Pgs -i \"input.avs\" -s 1080 -r 23 -n 18000 \"output.pgs\"\n"
		"\t-i <filename>\n"
		"\t\t Input file name. Use '-' for a stdin raw input. \n"
		"\t-s <format>\n"
		"\t\t Frame format:\n"
		"\t\t 480i   = 1/240/-480\n"
		"\t\t 576i   = 2/288/-576\n"
		"\t\t 480p   = 3/480\n"
		"\t\t 1080i  = 4/540/-1080\n"
		"\t\t 720p   = 5/720\n"
		"\t\t 1080p  = 6/1080\n"
		"\t\t 576p   = 7/576\n"
		"\t-r <rate>\n"
		"\t\t Frame rate:\n"
		"\t\t 23.976 = 1/23\n"
		"\t\t 24.0   = 2/24\n"
		"\t\t 25.0   = 3/25\n"
		"\t\t 29.97  = 4/29\n"
		"\t\t 30.0   = 5/30\n"
		"\t\t 50.0   = 6/50\n"
		"\t\t 59.94  = 7/59\n"
		"\t\t 60.0   = 8/60\n"
		"\t-b <frameid>\n"
		"\t\t Beginning index of valid frames.\n"
		"\t-e <count>\n"
		"\t-n <count>\n"
		"\t\t count of frames.\n"
		"\t-z[0|1]\n"
		"\t\t Setting up an extra epoch at the very beginning or not.\n"
		"\t\t 0      = No;\n"
		"\t\t 1      = Yes;\n"
		"\t\t (blank)= Yes. (default)\n"
		"\t-x[0|1]\n"
		"\t\t Using extremely strict mode or not.\n"
		"\t\t 0      = No;\n"
		"\t\t 1      = Yes;\n"
		"\t\t (blank)= Yes. (default)\n"
		"\t-v[level]\n"
		"\t\t Output level.\n"
		"\t\t (blank)= All, verbose;\n"
		"\t\t 63     = Errors only.\n"
		"\t\t 127    = Errors and warnings.\n"
		"\t\t 144    = Normal outputs. (default)\n"
		"\t<filename>\n"
		"\t\t Output file name. Use '-' for a stdout output.\n\n"
		));
}

int _tmain(int argc, _TCHAR* argv[])
{
	A2PStdLogger tLogger{ A2PLogger::info + A2PLogger::normal };
	A2PClaSettings tSettings {};

	LPTSTR input = nullptr;
	LPTSTR output = nullptr;
	BdViFormat format = BdViFormat::Unknown;
	BdViFrameRate rate = BdViFrameRate::Unknown;
	int begin = -1;
	int end = -1;

	static TCHAR const * optlist = _T("hi:s:r:n:b:e:z::x::v::");
	opterr = 0;
	for (;;)
	{
		int iOpt = getopt(argc, argv, optlist);
		if (iOpt == -1) break;

		bool tFlag;
		int tValue;

		switch (iOpt)
		{
		case 'h':
			print_usage(&tLogger);
			return 0;
		case 'i':
			input = optarg;
			break;
		case 's':
			format = GetFormat(_ttoi(optarg));
			break;
		case 'r':
			rate = GetRate(_ttoi(optarg));
			break;
		case 'b':
			begin = _ttoi(optarg);
			break;
		case 'z':
			tFlag = true;
			if(optarg != NULL)
			{
				tFlag = (_ttoi(optarg) != 0);
			}
			tSettings.SetForcingEpochZeroStart(tFlag);
			break;
		case 'x':
			tFlag = true;
			if (optarg != NULL)
			{
				tFlag = (_ttoi(optarg) != 0);
			}
			tSettings.SetForcingTmtCompat(!tFlag);
			break;
		case 'v':
			tValue = A2PLogger::all;
			if (optarg != NULL)
			{
				tValue = _ttoi(optarg);
			}
			tLogger.SetLogLevel(tValue);
			break;
		case 'n':
		case 'e':
			end = _ttoi(optarg);
			break;
		default:
			break;
		}
	}

	bool misUse = false;

	if (argc - optind == 1)
	{
		output = argv[optind];
	}
	else
	{
		misUse = true;
	}

	misUse |= (format == BdViFormat::Unknown);
	misUse |= (rate == BdViFrameRate::Unknown);
	misUse |= (output == nullptr);
	if (misUse)
	{
		print_usage(&tLogger);
		return 1;
	}

	FILE * ofile = (*output == '-' && output[1] == '\0') ? stdout : _tfopen(output, _T("wb"));

	misUse |= (ofile == NULL);
	if (misUse)
	{
		print_usage(&tLogger);
		return 1;
	}
	
	CfileStreamEx ostream = CfileStreamEx(ofile, false, true, false, nullptr);
	CfileStreamEx istream = CfileStreamEx(stdin, true, false, false, nullptr);

	ClaAdvisor advisor{ format, rate, begin, end };

	std::auto_ptr<FrameStream> avstream(nullptr);

	int tRet = 0;
	try
	{
		if (*input == '-' && input[1] == '\0')
		{
			avstream.reset(new BgraRawStream(&istream, &advisor));
		}
		else
		{
			avstream.reset(new BgraAvsStream(input, &advisor));
		}

		A2PEncoder tA2P{ &tSettings , &tLogger };
		tRet = tA2P.Encode(avstream.get(), &ostream, nullptr);
		
		tLogger.Log(A2PLogger::info + A2PLogger::normal, _T("Done.\n"));

		return tRet;
	}
	catch (EndUserException ex)
	{
		tRet = 2;
		TCHAR buffer[A2PException::MaxBufferSize];
		ex.FormatMessage(buffer, A2PException::MaxBufferSize);
		tLogger.Log(A2PLogger::error, buffer);
	}
	catch (AvsInitException)
	{
		tRet = 2;
		tLogger.Log(A2PLogger::error, _T("Input AvStream file open failed.\n"));
	}
	catch (...)
	{
		tRet = 3;
		tLogger.Log(A2PLogger::error, _T("Unknown exception."));
	}

	tLogger.Log(A2PLogger::info + A2PLogger::normal, _T("\nConverting failed.\n"));
	return tRet;
}
