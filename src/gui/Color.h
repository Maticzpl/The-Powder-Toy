#pragma once

namespace gui
{
	template<class Type>
	struct XColor
	{
		Type r, g, b, a;
	};

	using Color = XColor<int>;
	using FColor = XColor<float>;
};
