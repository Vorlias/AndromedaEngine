#pragma once
#include <atomic>

namespace andromeda {
    class RefCounted;
    
    // A reference to a ref counted object
	template<typename T>
	class Ref {
	public:
		Ref() = default;
		Ref(std::nullptr_t n) : ptr(nullptr) {}
		Ref(T* ptr) : ptr(ptr) {
			static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted!");
			Increment();
		}

		template<typename T2>
			requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
		Ref(Ref<T2>&& other) {
			ptr = (T*)other.ptr;
			other.ptr = nullptr;
		}

		Ref(const Ref<T>& other) : ptr(other.ptr) {
			Increment();
		}

		~Ref() {
			Decrement();
		}

		template<typename... Args>
		static Ref<T> Create(Args&&... args) {
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

		const size_t GetRefCount() const {
			return ptr->GetRefCount();
		}

		inline void Reset(T* instance = nullptr) {
			Decrement();
			ptr = instance;
		}

        T* AsPtr() { return ptr; }

        template<typename T2>
        requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
        Ref<T2> As() const {
            return Ref<T2>(*this);
        }

		T* operator->() {
			return ptr;
		}

		const T* operator->() const {
			return ptr;
		}

		T& operator*() {
			return ptr;
		}

		const T& operator*() const {
			return ptr;
        }

        operator bool() { return ptr != nullptr; }
        operator bool() const { return ptr != nullptr; }

        bool operator==(const Ref<T>& other) {
            return ptr == other.ptr;
        }

        bool operator!=(const Ref<T>& other) {
            return !(*this == other);
        }
	private:
		inline void Increment() {
			if (ptr) {
				ptr->IncRefCount();
			}
		}

		inline void Decrement() {
			if (ptr != nullptr) {
				ptr->DecRefCount();

				if (ptr->GetRefCount() == 0) {
					delete ptr;
					ptr = nullptr;
				}
			}
		}

		mutable T* ptr;
	};

	class RefCounted {
	public:
		RefCounted() = default;
		virtual ~RefCounted() = default;

	private:
        template<typename T2>
		friend class Ref;

		void IncRefCount() const {
			++m_RefCount;
		}

		void DecRefCount() const {
			--m_RefCount;
		}

		uint32_t GetRefCount() const {
			return m_RefCount.load();
		}
		mutable std::atomic<uint32_t> m_RefCount = 0;
	};

    
	template<typename T, typename ... Args>
	DEPRECATED constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return Ref<T>::Create(std::forward<Args>(args)...);
	}
} // namespace andromeda