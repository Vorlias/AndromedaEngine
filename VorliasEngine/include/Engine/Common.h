#pragma once
#include <memory>
#include "Engine/Data/Vector.h"

namespace andromeda {
	template<typename T>
	using ScopeRef = std::unique_ptr<T>;

	template<typename T>
	using SharedRef = std::shared_ptr<T>;

	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	template<typename T, typename ... Args>
	constexpr SharedRef<T> CreateSharedRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	constexpr ScopeRef<T> CreateScopeRef(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}