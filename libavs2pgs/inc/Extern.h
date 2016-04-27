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

#include "GraphicalTypes.h"

static int const Colors = 254;
typedef void* quantizer_t;

extern "C" quantizer_t * new_quantizer();
extern "C" void insert_color(quantizer_t *q, avs2pgs::Pixel32 color);
extern "C" int get_palette(quantizer_t *q, avs2pgs::Pixel32 pal[Colors + 1]);
extern "C" int get_color_index(quantizer_t *q, avs2pgs::Pixel32 color);
extern "C" void destroy_quantizer(quantizer_t *q);

extern "C" int asm_is_identical_sse2(int length, char *img, char *img_old);
extern "C" int asm_is_empty_sse2(int length, char *img);
extern "C" void asm_zero_transparent_sse2(int length, char volatile *img);
//extern "C" void asm_swap_rb_sse2(int length, char volatile *img, char volatile *out);

