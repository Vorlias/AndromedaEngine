#include "ObjectPool.h"
#include <algorithm>


///////////////////////////
//  POOL                //
//////////////////////////

template<typename T>
andromeda::ManagedPool<T>::ManagedPool(size_t maxSize) : m_pool(0), m_maxSize(maxSize) {}

template<typename T>
andromeda::ManagedPool<T>::ManagedPool(size_t maxSize, size_t initSize) : m_maxSize(maxSize) {
	if (initSize > maxSize) {
		initSize = maxSize;
	}

	m_pool.reserve(initSize);
	for (size_t i = 0; i < initSize; i++) {
		m_pool.emplace_back(std::move(std::make_unique<T>()));
	}
}

template<typename T>
andromeda::ManagedPool<T>::Object andromeda::ManagedPool<T>::acquire() {
	if (!m_pool.empty()) {
		std::unique_ptr<T> obj(std::move(m_pool.back()));
		m_pool.pop_back();

		m_used++;
		Object objSmart(obj.release(), [this](T* t) {
			if (m_pool.size() < m_maxSize) {
				m_pool.push_back(std::unique_ptr<T>(t));
				m_used--;
			}
		});

		return objSmart;
	}

	if (m_used < m_maxSize) {
		T* obj = new T();
		m_used++;
		Object objSmart(obj, [this](T* t) {
			if (m_pool.size() < m_maxSize) {
				m_pool.push_back(std::unique_ptr<T>(t));
				m_used--;
			}
		});

		return objSmart;
	}

	return nullptr;
}

template<typename T>
size_t andromeda::ManagedPool<T>::size() const {
	return m_pool.size();
}

template<typename T>
size_t andromeda::ManagedPool<T>::allocated() const {
	return m_used;
}

template<typename T>
andromeda::ManagedPool<T>::~ManagedPool() {
	m_pool.clear();
}


///////////////////////////
//  PTR POOL             //
//////////////////////////
template<typename T>
andromeda::Pool<T>::Pool(size_t maxSize) : m_freePool(0), m_maxSize(maxSize) {}

template<typename T>
andromeda::Pool<T>::Pool(size_t maxSize, size_t initSize) : m_maxSize(maxSize) {
	if (initSize > maxSize) {
		initSize = maxSize;
	}

	m_freePool.reserve(initSize);
	for (size_t i = 0; i < initSize; i++) {
		m_freePool.emplace_back(new T());
	}
}

template<typename T>
T* andromeda::Pool<T>::acquire() {
    if (!m_freePool.empty()) {
        T* top = m_freePool.back();
        m_freePool.pop_back();

        m_usedPool.push_back(top);
        return top;
    }

    if (m_usedPool.size() < m_maxSize) {
        T* obj = new T();
        m_usedPool.push_back(obj);
        return obj;
    }

	return nullptr;
}

template<typename T>
void andromeda::Pool<T>::release(T* value) { 
    if (m_freePool.size() > m_maxSize) {
        delete value;
        return;
    }

    const auto res = std::find(m_usedPool.begin(), m_usedPool.end(), value);
    if (res != m_usedPool.end()) {
        m_usedPool.erase(res);
    }

    m_freePool.push_back(value);
}

template<typename T>
andromeda::Pool<T>::~Pool() {
	m_freePool.clear();
}