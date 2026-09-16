#pragma once
#include <vector>
#include <memory>

namespace andromeda {
	// A pool of objects - any objects _used_ will be returned to this pool when no longer referenced
	template<typename T>
	class ManagedPool {
	public:
		using Object = std::shared_ptr<T>;
		typedef T object_t;

		ManagedPool(size_t maxSize);
		ManagedPool(size_t maxSize, size_t initSize);
		~ManagedPool();

		[[nodiscard]] Object acquire();
		[[nodiscard]] size_t size() const;
		[[nodiscard]] size_t allocated() const;

	private:
		std::vector<std::unique_ptr<T>> m_pool{};
		size_t m_maxSize{};
		size_t m_used{};
	};

    // A pool of object pointers
	template<typename T>
	class Pool {
	public:
        Pool(size_t maxSize);
		Pool(size_t maxSize, size_t initSize);
		~Pool();

        [[nodiscard]] T* acquire();
        void release(T* obj);
	private:
		std::vector<T*> m_freePool{};
        std::vector<T*> m_usedPool{};

		size_t m_maxSize{};
	};
} // namespace andromeda

#include "ObjectPool.inl"