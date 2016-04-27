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

namespace avs2pgs
{
	enum struct BdGxSegmentType : unsigned __int8
	{
		UnKnown,

		PDS = 0x14,
		ODS,
		PCS = 0x16,
		WDS = 0x17,
		ICS,
		END = 0x80,
		DSS,
		DPS,
		TRM = 0xFF
	};

	enum struct BdCompositionType : unsigned __int8
	{
		NormalCase,
		AcquisitionPoint = 1,
		EpochStart = 2,

		EpochContinue = 3
	};

	enum struct BdViFormat : unsigned __int8
	{
		Unknown,

		Vi480i = 0x01,
		Vi576i = 0x02,
		Vi480p = 0x03,
		Vi1080i = 0x04,
		Vi720p = 0x05,
		Vi1080p = 0x06,
		Vi576p = 0x07
	};

	enum struct BdViFrameRate : unsigned __int8
	{
		Unknown = 0x00,

		Vi23 = 0x01,
		Vi24 = 0x02,
		Vi25 = 0x03,
		Vi29 = 0x04,
		Vi30 = 0x05,
		Vi50 = 0x06,
		Vi59 = 0x07,
		Vi60 = 0x08
	};


	enum struct BdMuiStreamType : unsigned __int8
	{
		Unknown = 0,

		Video = 1,
		Audio = 2,
		Graphics = 3,
		Text = 4
	};
}