#pragma once
#include "Base/Main/Common.h"

namespace S2DE::Math
{
	template<class T>
	static T Max(T a, T b)
	{
		return max(a, b);
	}
}