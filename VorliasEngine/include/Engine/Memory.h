#pragma once
#include <atomic>

namespace andromeda {
	class RefCounted;

	template<class U, class T>
	struct is_explicitly_convertible {
		enum { value = std::is_constructible<T, U>::value && !std::is_convertible<U, T>::value };
	};

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

		constexpr Ref<T>& operator=(const Ref<T>& lhs) {
			this->ptr = lhs.ptr;
			Increment();
			return *this;
		}

		constexpr Ref<T>& operator=(const std::nullptr_t lhs) {
			Reset();
			return *this;
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
			if (ptr == nullptr)
				return 0;
			return ptr->GetRefCount();
		}

		inline void Reset(T* instance = nullptr) {
			Decrement();
			ptr = instance;
		}

		T* AsPtr() {
			return ptr;
		}

		template<typename T2>
		T2* AsPtr() {
			return static_cast<T2*>(ptr);
		}

		template<typename T2>
			requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
		Ref<T2> As() const {
			return {(T2*)this->ptr};
		}

		template<typename T2>
		bool IsCastable() const {
			return is_explicitly_convertible<T, T2>::value;
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

		operator bool() {
			return ptr != nullptr;
		}
		operator bool() const {
			return ptr != nullptr;
		}

		bool operator==(const Ref<T>& other) {
			return ptr == other.ptr;
		}

		bool operator!=(const Ref<T>& other) {
			return !(*this == other);
		}

	protected:
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

		mutable T* ptr{nullptr};
	};

	class RefCounted {
	public:
		RefCounted() = default;
		virtual ~RefCounted() = default;

	public:
		uint32_t GetRefCount() const {
			return m_RefCount.load();
		}

	private:
		template<typename T2>
		friend class Ref;

		void IncRefCount() const {
			++m_RefCount;
		}

		void DecRefCount() const {
			--m_RefCount;
		}


		mutable std::atomic<uint32_t> m_RefCount = 0;
	};


	template<typename T, typename... Args>
	DEPRECATED constexpr Ref<T> CreateRef(Args&&... args) {
		return Ref<T>::Create(std::forward<Args>(args)...);
	}
} // namespace andromeda