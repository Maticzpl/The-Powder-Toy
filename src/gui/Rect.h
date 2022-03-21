#pragma once

#include "Point.h"

namespace
{
	template<class Type>
	constexpr Type Min(Type a, Type b)
	{
		return a < b ? a : b;
	}

	template<class Type>
	constexpr Type Max(Type a, Type b)
	{
		return a > b ? a : b;
	}

	template<class Type>
	constexpr Type Clamp(Type v, Type l, Type h)
	{
		return Max(Min(v, h), l);
	}
}

namespace gui
{
	template<class Type>
	struct XRect
	{
		using Point = XPoint<Type>;

		Point pos, size;

		constexpr bool Contains(Point point) const
		{
			return point.x >= pos.x          &&
			       point.y >= pos.y          &&
			       point.x <  pos.x + size.x &&
			       point.y <  pos.y + size.y;
		}

		constexpr XRect Offset(Point offset) const
		{
			return { pos + offset, size };
		}

		constexpr XRect Grow(Point grow) const
		{
			return { pos, size + grow };
		}

		constexpr XRect Rebase(Point rebase) const
		{
			return { pos + rebase, size - rebase };
		}

		static constexpr XRect FromCorners(Point one, Point other)
		{
			return {
				{
					::Min(one.x, other.x),
					::Min(one.y, other.y),
				},
				{
					::Max(one.x, other.x) - ::Min(one.x, other.x) + 1,
					::Max(one.y, other.y) - ::Min(one.y, other.y) + 1,
				},
			};
		}

		constexpr Point TopLeft() const
		{
			return pos;
		}

		constexpr Point TopRight() const
		{
			return pos + Point{ size.x - 1, 0 };
		}

		constexpr Point BottomLeft() const
		{
			return pos + Point{ 0, size.y - 1 };;
		}

		constexpr Point BottomRight() const
		{
			return pos + Point{ size.x - 1, size.y - 1 };
		}

		constexpr Point Clamp(Point point) const
		{
			return {
				::Clamp(point.x, pos.x, pos.x + size.x - 1),
				::Clamp(point.y, pos.y, pos.y + size.y - 1),
			};
		}

		constexpr XRect Intersect(XRect other) const
		{
			return FromCorners(Clamp(other.TopLeft()), Clamp(other.BottomRight()));
		}

		constexpr bool operator ==(XRect other) const
		{
			return pos == other.pos && size == other.size;
		}

		constexpr bool operator !=(XRect other) const
		{
			return !(*this == other);
		}
	};

	using Rect = XRect<int>;
	using FRect = XRect<float>;
}
