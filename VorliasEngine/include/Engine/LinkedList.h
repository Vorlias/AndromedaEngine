#pragma once
#include "Common.h"

#define MAKE_PTR(T, ...) new T(__VA_ARGS__) // std::make_shared<T>(__VA_ARGS__)

namespace andromeda {
	class ListIndexException : public std::exception {
	public:
		virtual const char* what() const throw() {
			return "List is out of range (value == it.end())";
		}
	};

	template<typename T>
	class LinkedList {
	public:
		struct Node {
			static SharedRef<Node> Create(T value) {
				return std::make_shared<Node>(value);
			}

			T value;
			SharedRef<Node> next;
			SharedRef<Node> prev;

			const T& operator*() const {
				return value;
			}

			const T* operator&() const {
				return &value;
			}

			SharedRef<Node> operator++() const {
				return next;
			}

			SharedRef<Node> operator--() const {
				return prev;
			}

			Node(T value) : value(value) {}
		};
		typedef SharedRef<Node> NodePtr;

		class Iterator {
		public:
			const LinkedList<T>* list{};
			NodePtr ptr{};

			Iterator(const LinkedList<T>* list, NodePtr value) : list(list), ptr(value) {};

			Iterator operator++() {
				ptr = ptr->next;
				return *this;
			}

			Iterator operator++(int) {
				ptr = ptr->next;
				return *this;
			}

			bool operator==(const Iterator& other) const {
				return ptr == other.ptr;
			}

			bool operator!=(const Iterator& other) const {
				return ptr != other.ptr;
			}

			operator bool() const {
				return ptr != nullptr;
			}

			T& operator*() const {
				return ptr->value;
			}

			T& value() const {
				return ptr->value;
			}
		};


		class ReverseIterator {
		public:
			const LinkedList<T>* list;
			NodePtr ptr{};

			ReverseIterator(const LinkedList<T>* list, NodePtr value) : list(list), ptr(value) {};

			ReverseIterator operator++() {
				ptr = ptr->prev;
				return *this;
			}

			ReverseIterator operator++(int) {
				ptr = ptr->prev;
				return *this;
			}

			bool operator==(const Iterator& other) const {
				return ptr == other.ptr;
			}

			bool operator!=(const ReverseIterator& other) const {
				return ptr != other.ptr;
			}

			T& operator*() const {
				return ptr->value;
			}
		};

	private:
	public:
		LinkedList() = default;

		LinkedList(const T& value) {
			push_back(value);
		}

		LinkedList(std::initializer_list<T> values) {
			for (auto it = values.begin(); it != values.end(); it++) {
				push_back(*it);
			}
		}

		void push_back(T value) {
			NodePtr node = Node::Create(value);
			if (_tail != nullptr) {
				_tail->next = node;
				node->prev = _tail;
			} else {
				_head = node;
			}
			_tail = node;
			_count += 1;
		}

		void push_front(T value) {
			NodePtr node = Node::Create(value);
			if (_head != nullptr) {
				_head->prev = node;
				node->next = _head;
			} else {
				_tail = node;
			}
			_head = node;
			_count += 1;
		}

		void move_back(LinkedList<T>& other) {
			_tail->next = other._head;
			_tail = other._tail;
			_count += other._count;

			other._count = 0;
			other._tail = nullptr;
			other._head = nullptr;
		}

		void move_front(LinkedList<T>& other) {
			_head->prev = other._tail;
			other._tail->next = _head;

			_head = other._head;
			_count += other._count;

			other._count = 0;
			other._tail = nullptr;
			other._head = nullptr;
		}

		void push_back(const LinkedList<T>& other) {
			for (auto it = other.begin(); it != other.end(); ++it) {
				push_back(*it);
			}
		}

		void push_front(const LinkedList<T>& other) {
			for (auto it = other.begin(); it != other.end(); ++it) {
				push_front(*it);
			}
		}

		void erase(Iterator position) {
			ANDROMEDA_ASSERT(position.list == this);

			NodePtr toRemove = position.ptr;
			if (_head == toRemove) {
				_head = toRemove->next;
			}

			if (_tail == toRemove) {
				_tail = toRemove->prev;
			}

			if (toRemove->next != nullptr) {
				toRemove->next->prev = toRemove->prev;
			}

			if (toRemove->prev != nullptr) {
				toRemove->prev->next = toRemove->next;
			}

			_count -= 1;
			position.ptr = nullptr;
		}


		// Erase from the position given to
		void erase(Iterator position, size_t count) {
			size_t i{0};

			for (; position != end(); position++) {
				remove(position);
				i++;

				if (i >= count)
					break;
			}
		}

		// Find the given value in this linked list that matches the given predicate
		const Iterator find(std::function<bool(const T&)> predicate) const {
			for (Iterator it = begin(); it != end(); it++) {
				if (predicate(*it))
					return it;
			}

			return end();
		}

		// Find the given value in this linked list
		const Iterator find(const T& value) {
			for (Iterator it = begin(); it != end(); it++) {
				if (*it == value)
					return it;
			}

			return end();
		}

		Iterator begin() const {
			return Iterator(this, _head);
		}

		Iterator end() const {
			return Iterator(this, nullptr);
		}

		ReverseIterator rbegin() const {
			return ReverseIterator(this, _tail);
		}

		ReverseIterator rend() const {
			return ReverseIterator(this, nullptr);
		}

		NodePtr tail() const {
			return _tail;
		}

		NodePtr head() const {
			return _head;
		}

		constexpr size_t size() const {
			return _count;
		}

	private:
		NodePtr _head{};
		NodePtr _tail{};
		size_t _count{0};
		bool _readonly = false;
	};
} // namespace andromeda