#ifndef ANDROMEDA_PCH
#define ANDROMEDA_PCH

#define ENGINE_NS andromeda
#define USING_ENGINE using namespace ENGINE_NS;

#include <cstdint>
#include <memory>

#include <string>
#include <iostream>

// Collections
#include <array>
#include <vector>
#include <queue>
#include <stack>

#ifdef _WIN32
#	define ANDROMEDA_WIN 1

#	ifdef _WIN64
#		define ANDROMEDA_WIN64 1
#	else
#		define ANDROMEDA_WIN32 1
#		error "x86 builds are not supported"
#	endif
#elif defined(__ANDROID__)
#	define ANDROMEDA_ANDROID 1
#	error "Android not supported"
#elif defined(__linux__)
#	define ANDROMEDA_LINUX 1
#elif defined(__APPLE__) || defined(__MACH__)
#	include <TargetConditionals.h>

#	if TARGET_IPHONE_SIMULATOR == 1
#		error "IOS simulator is not supported!"
#	elif TARGET_OS_IPHONE == 1
#		define ANDROMEDA_IPHONE 1
#		error "IOS is not supported!"
#	elif TARGET_OS_MAC == 1
#		define ANDROMEDA_MAC 1
#	else
#		error "Unknown Apple platform!"
#	endif
#else
#	error "Unknown platform!"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#	define PLATFORM_ARCH_X64 1
#	define ANDROMEDA_X64 1
#elif defined(__i386__) || defined(_M_IX86)
#	define PLATFORM_ARCH_X86 1
#	define ANDROMEDA_X32 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#	define PLATFORM_ARCH_ARM64 1
#	define ANDROMEDA_X64 1
#elif defined(__arm__) || defined(_M_ARM)
#	define PLATFORM_ARCH_ARM 1
#	define ANDROMEDA_X32 1
#else
#	error "Unsupported architecture"
#endif

#if defined(__clang__)
#	define ANDROMEDA_CXX_COMPILER "clang"
#elif defined(__GNUC__)
#	define ANDROMEDA_CXX_COMPILER "GCC"
#elif defined(_MSC_VER)
#	define ANDROMEDA_CXX_COMPILER "MSVC"
#else
#	define ANDROMEDA_CXX_COMPILER "Unknown"
#endif

#if _WIN32
#	define DEPRECATED __declspec(deprecated)
#	define DEPRECATED_S(R) __declspec(deprecated(R))
#else
#	define DEPRECATED [[deprecated]]
#	define DEPRECATED_S(R) [[deprecated(R)]]
#endif

#endif // PCH_H