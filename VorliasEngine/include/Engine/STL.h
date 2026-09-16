#pragma once
// standard library utils

template<class U, class T>
struct is_explicitly_convertible {
	enum { value = std::is_constructible<T, U>::value && !std::is_convertible<U, T>::value };
};