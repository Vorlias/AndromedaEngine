#pragma once

#define MAKE_PTR(T, ...) new T(__VA_ARGS__) // std::make_shared<T>(__VA_ARGS__)

namespace andromeda {
	class ListIndexException : public std::exception {
	public:
		virtual const char* what() const throw() {
			return "List is out of range (value == it.end())";
		}
	};



	template<typename T>
	struct ListNode {
		using Node = ListNode<T>*;

		T value;

		Node next{};
		Node prev{};

		ListNode(T value) : value(value) {}
	};

	// A linked list
	template<typename T>
	struct List {
		class Iterator {
		public:
			const List<T>* list{};
			ListNode<T>* ptr{};

			Iterator(const List<T>* list, ListNode<T>* value) : list(list), ptr(value) {};

			Iterator operator++() {
				ptr = ptr->next;
				return *this;
			}

			Iterator operator++(int) {
				ptr = ptr->next;
				return *this;
			}

			Iterator operator+(int value) {
				if (value > 0) {
					for (int i = 0; i < value; i++) {
						ptr = ptr->next;
						if (ptr == nullptr)
							return *this;
					}
				} else if (value < 0) {
					for (int i = 0; i < -value; i++) {
						ptr = ptr->prev;
						if (ptr == nullptr)
							return *this;
					}
				}

				return *this;
			}

			Iterator operator-(int value) {
				return (*this) + (-value);
			}

			bool operator==(const Iterator& other) {
				return ptr == other.ptr;
			}

			bool operator!=(const Iterator& other) {
				return ptr != other.ptr;
			}

			operator bool() const {
				return ptr != nullptr && list->size() > 0;
			}

			const T& operator*() const {
				return ptr->value;
			}
		};
		class ReverseIterator {
		public:
			const List<T>* list;
			ListNode<T>* ptr;

			ReverseIterator(const List<T>* list, ListNode<T>* value) : list(list), ptr(value) {};

			ReverseIterator operator++() {
				ptr = ptr->prev;
				return *this;
			}

			ReverseIterator operator++(int) {
				ptr = ptr->prev;
				return *this;
			}

			bool operator!=(const ReverseIterator& other) {
				return ptr != other.ptr;
			}

			const T& operator*() const {
				return ptr->value;
			}
		};

		List() = default;
		List(const T& value) {
			push_back(value);
		}

		List(std::initializer_list<T> values) {
			for (auto it = values.begin(); it != values.end(); it++) {
				push_back(*it);
			}
		}

		List(List<T>& src) = delete;

		List(List<T>&& src) {
			std::cout << " move list " << std::endl;

			m_head = std::move(src.m_head);
			m_tail = std::move(src.m_tail);
			count = src.count;

			src.m_head = nullptr;
			src.m_tail = nullptr;
			src.count = 0;
		}

		List(const List<T>& src) {
			for (auto it = src.begin(); it != src.end(); ++it) {
				push_back(*it);
			}
		}

		void push_back(T value) {
			ListNode<T>* node = new ListNode<T>(value);
			if (m_tail != nullptr) {
				m_tail->next = node;
				node->prev = m_tail;
			} else {
				m_head = node;
			}
			m_tail = node;
			count += 1;
		}

		// // push back by value
		// void push_back(List<T>& other) {

		// }

		// push back by ref
		void push_back(const List<T>& other) {
			for (auto it = other.begin(); it != other.end(); ++it) {
				push_back(*it);
			}
		}

		// push front by value
		void push_front(const List<T>& other) {
			for (auto it = other.rbegin(); it != other.rend(); ++it) {
				push_front(*it);
			}
		}

		void push_front(T value) {
			ListNode<T>* node = new ListNode<T>(value);
			if (m_head != nullptr) {
				m_head->prev = node;
				node->next = m_head;
			} else {
				m_tail = node;
			}
			m_head = node;
			count += 1;
		}

		// void swap(Iterator a, Iterator b) {
		// 	ANDROMEDA_ASSERT(a.list == this);
		// 	ANDROMEDA_ASSERT(b.list == this);
		// }

		void erase_front(Iterator position, size_t count) {
			size_t i{0};

			for (; position != end(); position++) {
				remove(position);
				i++;

				if (i >= count)
					break;
			}
		}

		void erase_back(ReverseIterator position, size_t count) {
			size_t i{0};

			for (; position != rend(); position++) {
				remove(position);
				i++;

				if (i >= count)
					break;
			}
		}

		void remove(Iterator position) {
			ANDROMEDA_ASSERT(position.list == this);

			ListNode<T>* toRemove = position.ptr;
			if (m_head == toRemove) {
				m_head = toRemove->next;
			}

			if (m_tail == toRemove) {
				m_tail = toRemove->prev;
			}

			if (toRemove->next != nullptr) {
				toRemove->next->prev = toRemove->prev;
			}

			if (toRemove->prev != nullptr) {
				toRemove->prev->next = toRemove->next;
			}

			// finally delete this node
			/// delete toRemove;
			count -= 1;

			position.ptr = nullptr;
		}

		const Iterator find(const T& value) {
			for (Iterator it = begin(); it != end(); it++) {
				if (*it == value)
					return it;
			}

			return end();
		}

		const ReverseIterator rfind(const T& value) {
			for (ReverseIterator it = begin(); it != end(); it++) {
				if (*it == value)
					return it;
			}

			return end();
		}

		Iterator begin() const {
			return Iterator(this, m_head);
		}

		Iterator end() const {
			return Iterator(this, nullptr);
		}

		ReverseIterator rbegin() const {
			return ReverseIterator(this, m_tail);
		}

		ReverseIterator rend() const {
			return ReverseIterator(this, nullptr);
		}

		operator bool() const {
			return m_head != nullptr && m_tail != nullptr;
		}

		void clear() {
			for (Iterator it = begin(); it != end(); it++) {
				delete it.ptr;
			}
			m_head = nullptr;
			m_tail = nullptr;
		}

		~List() {
			// clear();
		}

		const ListNode<T>* head() const {
			return m_head;
		}
		const ListNode<T>* tail() const {
			return m_tail;
		}
		const size_t size() const {
			return count;
		}

		T operator[](int i) {
			Iterator v = begin() + i;
			if (v == end())
				throw ListIndexException();
			return *v;
		}

	private:
		ListNode<T>::Node m_head{};
		ListNode<T>::Node m_tail{};
		size_t count{0};
	};
} // namespace andromeda