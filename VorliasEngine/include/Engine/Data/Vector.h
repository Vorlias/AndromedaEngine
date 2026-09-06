#pragma once
#include <math.h>
#include "lualib.h"
#include "Engine/Math.h"

struct lua_State;

static constexpr float kEpsilon = 0.00001f;
namespace andromeda {
	struct Vector3;
	struct Vector2 {
		float x{}, y{};

		Vector2() = default;

		explicit Vector2(float x, float y) : x(x), y(y) {}
		explicit Vector2(double x, double y): x(static_cast<float>(x)), y(static_cast<float>(y)) {}
		explicit Vector2(int x, int y): x(static_cast<float>(x)), y(static_cast<float>(y)) {}

		Vector2 Normalize() const;
		float GetMagnitude() const;
		float GetDistance(Vector2 other) const;
		Vector3 Extend(float z = 0);

		Vector2 operator+(Vector2 other) const {
			return Vector2(x + other.x, y + other.y);
		}

		Vector2 operator-(Vector2 other) const {
			return Vector2(x - other.x, y - other.y);
		}

		Vector2 operator/(float r) const {
			return Vector2(x / r, y / r);
		}

		Vector2 operator*(float r) const {
			return Vector2(x * r, y * r);
		}

		bool operator==(Vector2 other) {
			return x == other.x && y == other.y;
		}

		constexpr Vector2 WithX(float x) const { return Vector2(x, y); }
		constexpr Vector2 WithY(float y) const { return Vector2(x, y); }

		static Vector2 zero;
		static Vector2 one;

		static Vector2 up;
		static Vector2 down;
		static Vector2 left;
		static Vector2 right;
	};

	void registerVector2Lib(lua_State* L);
	void registerVector3Lib(lua_State* L);

	struct Vector3 {
		float x, y, z;

		Vector3() = default;
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		Vector3 Normalize() const;
		float GetMagnitude() const;
		float GetDistance(Vector3 other) const;

		Vector2 Shrink();
		constexpr Vector3 WithX(float x) const { return Vector3(x, y, z); }
		constexpr Vector3 WithY(float y) const { return Vector3(x, y, z); }
		constexpr Vector3 WithZ(float z) const { return Vector3(x, y, z); }

		Vector3 operator+(Vector3 other) const {
			return Vector3(x + other.x, y + other.y, z + other.z);
		}

		Vector3 operator-(Vector3 other) const {
			return Vector3(x - other.x, y - other.y, z - other.z);
		}

		Vector3 operator/(float r) const {
			return Vector3(x / r, y / r, z / r);
		}

		Vector3 operator*(float r) const {
			return Vector3(x * r, y * r, y * r);
		}

		bool operator==(Vector3 other) {
			return x == other.x && y == other.y && z == other.z;
		}

		static Vector3 zero;
		static Vector3 one;

		static Vector3 up;
		static Vector3 down;
		static Vector3 left;
		static Vector3 right;
		static Vector3 forward;
		static Vector3 backward;
	};

	// two dimensional unsigned-based vector
	struct Vector2u {
		uint32_t x{}, y{};

		Vector2u() = default;
		Vector2u(uint32_t x, uint32_t y) : x(x), y(y) {}
		Vector2u(const Vector2& vec): x(static_cast<uint32_t>(vec.x)), y(static_cast<uint32_t>(vec.y)) {}
	};

	// two dimensional integer-based vector
	struct Vector2i {
		int32_t x{}, y{};

		Vector2i() = default;
		Vector2i(int32_t x, int32_t y) : x(x), y(y) {}
		Vector2i(const Vector2& vec): x(static_cast<int32_t>(vec.x)), y(static_cast<int32_t>(vec.y)) {}

		Vector2i operator+(Vector2i other) const {
			return Vector2i(x + other.x, y + other.y);
		}

		Vector2i operator-(Vector2i other) const {
			return Vector2i(x - other.x, y - other.y);
		}

		Vector2i operator/(int r) const {
			return Vector2i(x / r, y / r);
		}

		Vector2i operator*(int r) const {
			return Vector2i(x * r, y * r);
		}
	};

	// three dimensional integer-based vector
	struct Vector3i {
		int32_t x{}, y{}, z{};

		Vector3i() = default;
		Vector3i(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z) {}
	};

	enum class VectorFormatStyle {
		Luau,
		CXX,
	};

	inline std::string to_string(const Vector2& vec, VectorFormatStyle format = VectorFormatStyle::CXX) {
		switch (format) {
			case VectorFormatStyle::Luau:
				char result[LUA_BUFFERSIZE];
				snprintf(result, sizeof(result), "%g, %g", vec.x, vec.y);
				return result;
			case VectorFormatStyle::CXX:
				return std::format("{:.2f}, {:.2f}", vec.x, vec.y);
		}
	}

	inline std::string to_string(const Vector3& vec, VectorFormatStyle format = VectorFormatStyle::CXX) {
		switch (format) {
			case VectorFormatStyle::Luau:
				char result[LUA_BUFFERSIZE];
				snprintf(result, sizeof(result), "%g, %g, %g", vec.x, vec.y, vec.z);
				return result;
			case VectorFormatStyle::CXX:
				return std::format("{:.2f}, {:.2f}, {:.2f}", vec.x, vec.y, vec.z);
		}
	}

	inline std::string to_string(const Vector2u& vec) {
		return std::format("{:d}u, {:d}u", vec.x, vec.y);
	}

	inline std::string to_string(const Vector2i& vec) {
		return std::format("{:d}, {:d}", vec.x, vec.y);
	}

	inline std::string to_string(const Vector3i& vec) {
		return std::format("{:d}, {:d}, {:d}", vec.x, vec.y, vec.z);
	}

	inline Vector3 radiansToDegrees(Vector3 radians) {
		return Vector3(radiansToDegrees(radians.x), radiansToDegrees(radians.y), radiansToDegrees(radians.z));
	}

	inline Vector3 radiansToDegreesAbs(Vector3 radians) {
		float x = radiansToDegreesAbs(radians.x);
		float y = radiansToDegreesAbs(radians.y);
		float z = radiansToDegreesAbs(radians.z);

		return Vector3(x, y, z);
	}

	inline Vector3 degreesToRadians(Vector3 degrees) {
		return Vector3(degreesToRadians(degrees.x), degreesToRadians(degrees.y), degreesToRadians(degrees.z));
	}
} // namespace andromeda


namespace andromeda_luau {
	void luaL_pushVector2(lua_State* L, const andromeda::Vector2& value);
	andromeda::Vector2* luaL_toVector2(lua_State* L, int idx);
} // namespace andromeda_luau