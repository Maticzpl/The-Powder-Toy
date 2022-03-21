#pragma once

#include <cassert>

namespace common
{
	template<class T>
	class ExplicitSingleton
	{
		static T *instance;

		ExplicitSingleton(const ExplicitSingleton &other) = delete;
		const ExplicitSingleton &operator =(const ExplicitSingleton &other) = delete;

	public:
		ExplicitSingleton()
		{
			assert(!instance);
			instance = static_cast<T *>(this);
		}
		
		~ExplicitSingleton()
		{
			instance = nullptr;
		}

		static T &Ref()
		{
			return *instance;
		}
	};

	template<class T>
	T *ExplicitSingleton<T>::instance = nullptr;
}
