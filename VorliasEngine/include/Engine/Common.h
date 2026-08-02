#pragma once
#include <memory>

namespace andromeda {
	template<typename T>
	using ScopeRef = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	constexpr ScopeRef<T> CreateScopeRef(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}