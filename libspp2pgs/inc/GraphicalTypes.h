/*----------------------------------------------------------------------------
* spp2pgs - Generates BluRay PG Stream from Subtitles or AviSynth scripts
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

#include <Windows.h>

namespace spp2pgs
{
	int const FrameWidthMax = 3840;
	int const FrameHeightMax = 2160;
	int const PaletteItemsMax = 256;

	int const SignBitMask = ~((unsigned int) -1 >> 1);

	struct Size final
	{
		int w, h;

		inline Size operator + (Size const &s) const { return Size{ w + s.w, h + s.h }; }
		inline Size operator - (Size const &s) const { return Size{ w - s.w, h - s.h }; }
		inline Size operator * (Size const &s) const { return Size{ w * s.w, h * s.h }; }
		inline Size operator / (Size const &s) const { return Size{ w / s.w, h / s.h }; }
		inline Size operator - () const { return Size{ -w, -h }; }

		inline Size operator * (int const &a) const { return Size{ w * a, h * a }; }
		inline Size operator / (int const &a) const { return Size{ w / a, h / a }; }

		inline bool operator == (Size const &s) const { return (w == s.w) && (h == s.h); }
		inline bool operator != (Size const &s) const { return !(*this == s); }

		inline bool operator >= (Size const &s) const { return (w >= s.w) && (h >= s.h); }
		inline bool operator > (Size const &s) const { return (w > s.w) && (h > s.h); }
		inline bool operator < (Size const &s) const { return (w < s.w) && (h < s.h); }
		inline bool operator <= (Size const &s) const { return (w <= s.w) && (h <= s.h); }
		
		inline int Area() const { return w * h; }
		inline Size PackUp(Size const &content) {
			int const &volume = content.Area();
			if (volume >= this->Area()) return *this;
			Size overlap = Size::MaxContents(*this, content);
			int const &overflow = volume - overlap.Area();
			if (overflow <= 0) return content;
			overlap.w += ((overlap.h == h) ? (overflow - 1) / h + 1 : 0);
			overlap.h += ((overlap.w == w) ? (overflow - 1) / w + 1 : 0);
			return overlap;
		}

		inline static Size MaxContents(Size const &s1, Size const &s2) { return Size{ min(s1.w, s2.w), min(s1.h, s2.h) }; }
		inline static Size MinContainer(Size const &s1, Size const &s2) { return Size{ max(s1.w, s2.w), max(s1.h, s2.h) }; }
		
	};

	struct Offset final
	{
		int dx, dy;

		inline Offset operator + (Offset const &o) const { return Offset{ dx + o.dx, dy + o.dy }; }
		inline Offset operator - (Offset const &o) const { return Offset{ dx - o.dx, dy - o.dy }; }
		inline Offset operator * (Size const &s) const { return Offset{ dx * s.w, dy * s.h }; }
		inline Offset operator / (Size const &s) const { return Offset{ dx / s.w, dy / s.h }; }
		inline Offset operator - () const { return Offset{ -dx, -dy }; }

		inline Offset operator * (int const &a) const { return Offset{ dx * a, dy * a }; }
		inline Offset operator / (int const &a) const { return Offset{ dx / a, dy / a }; }

		inline bool operator == (Offset const &o) const { return (dx == o.dx) && (dy == o.dy); }
		inline bool operator != (Offset const &o) const { return !(*this == o); }
	};

	struct Position final
	{
		int x, y;

		inline Position operator * (int const &a) const { return Position{ x * a, y * a }; }
		inline Position operator / (int const &a) const { return Position{ x / a, y / a }; }
		inline Position operator * (Size const &s) const { return Position{ x * s.w, y * s.h }; }
		inline Position operator / (Size const &s) const { return Position{ x / s.w, y / s.h }; }

		inline bool operator == (Position const &p) const { return (x == p.x) && (y == p.y); }
		inline bool operator != (Position const &p) const { return !(*this == p); }

		inline Position operator + (Offset const &o) const { return Position{ x + o.dx, y + o.dy }; }
		inline Position operator - (Offset const &o) const { return Position{ x - o.dx, y - o.dy }; }
		inline Offset operator - (Position const &p) const { return Offset{ x - p.x, y - p.y }; }

		inline Position operator - () const { return Position{ -x, -y }; }

		inline operator Offset () const { return Offset{ x, y }; }

		Position() {}
		Position(int const &x, int const &y) : x(x), y(y){}
		Position(Position const &p) : x(p.x), y(p.y){}
		Position(Offset const &o) : x(o.dx), y(o.dy){}
	};

	typedef Position Point;

	struct Border final
	{
		int left, top, right, bottom;

		Border & ToInnerBorder() { right--, bottom--; return *this; }
		Border & ToOuterBorder() { right++, bottom++; return *this; }
	};

	union Rect final
	{
		struct
		{
			Position pos;
			Size size;
		};
		struct{
			int x, y, w, h;
		};

		Rect() {}
		Rect(Rect const &rect) : pos(rect.pos), size(rect.size) {}
		Rect(int const &x, int const &y, int const &w, int const &h) : x(x), y(y), w(w), h(h) {}
		Rect(Position const &pos, Size const &size) : pos(pos), size(size) {}
		Rect(int const &x, int const &y, Size const &size) : x(x), y(y), size(size){}
		Rect(Position const &pos, int const &w, int const &h) : pos(pos), w(w), h(h) {}

		inline int Left() const { return x; }
		inline int Right() const { return x + w; }
		inline int Top() const { return y; }
		inline int Bottom() const { return y + h; }

		inline int Area() const { return size.Area(); }

		//scaling
		inline Rect operator * (int const &a) const { return Rect{ pos * a, size * a }; }
		inline Rect operator / (int const &a) const { return Rect{  pos / a, size / a  }; }
		inline Rect operator * (Size const &s) const { return Rect{  pos * s, size * s  }; }
		inline Rect operator / (Size const &s) const { return Rect{  pos / s, size / s  }; }

		//move
		inline Rect operator + (Offset const &o) const { return Rect{  pos + o, size  }; }
		inline Rect operator - (Offset const &o) const { return Rect{  pos - o, size  }; }

		//resize
		inline Rect operator + (Size const &s) const { return Rect{  pos, size + s }; }
		inline Rect operator - (Size const &s) const { return Rect{ pos, size - s }; }


		inline bool operator == (Rect const &r) const { return (pos == r.pos) && (size == r.size); }
		inline bool operator != (Rect const &r) const { return !(*this == r); }
		
		inline Border ToOuterBorder() const { return Border{ Left(), Top(), Right(), Bottom() }; }

		static inline Rect FromOuterBorder(int left, int top, int right, int bottom) { return Rect{ left, top ,  right - left, bottom - top }; }
		static inline Rect FromOuterBorder(Border b) { return FromOuterBorder(b.left, b.top, b.right, b.bottom); }
		
		inline Rect operator + (Rect const &r) const
		{
			return FromOuterBorder(min(Left(), r.Left()), min(Top(), r.Top()), max(Right(), r.Right()), max(Bottom(), r.Bottom()));
		}

		inline Rect operator * (Rect const &r) const
		{
			Rect tRet = FromOuterBorder(max(Left(), r.Left()), max(Top(), r.Top()), min(Right(), r.Right()), min(Bottom(), r.Bottom()));
			if ((tRet.w | tRet.h) & SignBitMask) tRet = Rect{ 0, 0, 0, 0 };
			return tRet;
		}
		
		inline bool IsContains(int const &posX, int const &posY) const
		{ 
			return 0 != (
				((posY - Top()) ^ (posY - Bottom())) & 
				((posX - Left()) ^ (posX - Right())) & 
				SignBitMask);
		}
		inline bool IsContainsBorder(int const &posX, int const &posY) const
		{
			return 0 != (
				((posY - Top()) ^ ~(Bottom() - posY)) &
				((posX - Left()) ^ ~(Right() - posX)) &
				SignBitMask);
		}

		inline bool IsContains(Point const &point) const { return IsContains(point.x, point.y); }
		inline bool IsContains(Rect const &rect) const { return IsContains(rect.x, rect.y) && IsContainsBorder(rect.Right(), rect.Bottom()); }

		inline bool IsIntersectsWith(Rect const &rect) const
		{
			return 0 != (
				((rect.Top() - Bottom()) ^ (rect.Bottom() - Top())) &
				((rect.Left() - Right()) ^ (rect.Right() - Left())) &
				SignBitMask);
		}

		inline bool IsContainsOrContainedBy(Rect const &rect) const
		{
			int dY1 = rect.Top() - Top();
			int dY2 = rect.Bottom() - Bottom();
			int dX1 = rect.Left() - Left();
			int dX2 = rect.Right() - Right();
			return  0 != ((dX1 ^ dX2) & (dY1 ^ dY2) & ~((dX1 ^ dY1) | (dX2 ^ dY2)) & SignBitMask);
		}
	};

	union Pixel32 final {
		struct
		{
			union
			{
				struct {
					unsigned __int8 r, g, b;
				} rgb;
				struct {
					unsigned __int8 b, g, r;
				} bgr;
				struct {
					unsigned __int8 y, cr, cb;
				} yuv;
			};

			unsigned __int8 a;
		};
		unsigned __int8 v[4];
		unsigned __int32 value;
	};

}