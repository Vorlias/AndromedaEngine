#pragma once
#include <math.h>
#include "lualib.h"
#include "Engine/Math.h"

struct lua_State;

static constexpr float kEpsilon = 0.00001f;
namespace andromeda {
	template<typename T>
		requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
	struct Vec2 {
		static constexpr uint32_t elements = 2;
		static constexpr size_t bytes = elements * sizeof(T);

		T x{}, y{};

		operator std::array<T, 2>() const {
			return {x, y};
		}

		bool operator==(const Vec2<T>& other) {
			return x == other.x && y == other.y;
		}
	};

	template<typename T>
		requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
	struct Vec3 {
		static constexpr uint32_t elements = 3;
		static constexpr size_t bytes = elements * sizeof(T);

		T x{}, y{}, z{};

		operator std::array<T, 3>() const {
			return {x, y, z};
		}

		bool operator==(const Vec3<T>& other) {
			return x == other.x && y == other.y && z == other.z;
		}
	};

	template<typename T>
		requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
	struct Vec4 {
		static constexpr uint32_t elements = 4;
		static constexpr size_t bytes = elements * sizeof(T);

		T x{}, y{}, z{}, w{};

		operator std::array<T, 4>() const {
			return {x, y, z, w};
		}

		bool operator==(const Vec4<T>& other) {
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}
	};

	struct Vector3;
	struct Vector2 : public Vec2<float> {
		Vector2() = default;

		explicit Vector2(float x, float y) : Vec2{x, y} {}
		explicit Vector2(double x, double y) : Vec2{static_cast<float>(x), static_cast<float>(y)} {}
		explicit Vector2(int x, int y) : Vec2{static_cast<float>(x), static_cast<float>(y)} {}

		Vector2 Normalize() const;
		float GetMagnitude() const;
		float GetDistance(Vector2 other) const;
		Vector3 Extend(float z = 0);

		Vector2 operator-() const {
			return Vector2(-x, -y);
		}

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

		constexpr Vector2 WithX(float x) const {
			return Vector2(x, y);
		}
		constexpr Vector2 WithY(float y) const {
			return Vector2(x, y);
		}

		static Vector2 zero;
		static Vector2 one;

		static Vector2 up;
		static Vector2 down;
		static Vector2 left;
		static Vector2 right;
	};

	void registerVector2Lib(lua_State* L);
	void registerVector3Lib(lua_State* L);

	struct Vector3 : public Vec3<float> {
		Vector3() = default;
		explicit Vector3(float x, float y, float z) : Vec3{x, y, z} {}
		explicit Vector3(int32_t x, int32_t y, int32_t z) : Vec3{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)} {}
		explicit Vector3(double x, double y, double z) : Vec3{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)} {}

		// explicit Vector3(const Vec3<int32_t>& i) : Vec3{static_cast<float>(i.x), static_cast<float>(i.y), static_cast<float>(i.z)} {}

		Vector3 Normalize() const;
		float GetMagnitude() const;
		float GetDistance(Vector3 other) const;

		Vector2 Shrink();

		constexpr Vector3 WithX(float x) const {
			return Vector3(x, y, z);
		}
		constexpr Vector3 WithY(float y) const {
			return Vector3(x, y, z);
		}
		constexpr Vector3 WithZ(float z) const {
			return Vector3(x, y, z);
		}

		Vector3 operator-() const {
			return Vector3(-x, -y, -z);
		}

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

		static Vector3 zero;
		static Vector3 one;

		static Vector3 up;
		static Vector3 down;
		static Vector3 left;
		static Vector3 right;
		static Vector3 forward;
		static Vector3 backward;

		static Vector3 Min(const Vector3& a, const Vector3& b);
		static Vector3 Max(const Vector3& a, const Vector3& b);
	};

	// two dimensional unsigned-based vector
	struct Vector2u : public Vec2<uint32_t> {
		Vector2u() = default;
		Vector2u(uint32_t x, uint32_t y) : Vec2{x, y} {}
		Vector2u(const Vector2& vec) : Vec2{static_cast<uint32_t>(vec.x), static_cast<uint32_t>(vec.y)} {}
	};

	// two dimensional integer-based vector
	struct Vector2i : public Vec2<int32_t> {
		Vector2i() = default;
		Vector2i(int32_t x, int32_t y) : Vec2{x, y} {}
		Vector2i(const Vector2& vec) : Vec2{static_cast<int32_t>(vec.x), static_cast<int32_t>(vec.y)} {}

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
	struct Vector3i : public Vec3<int32_t> {
		Vector3i() = default;
		Vector3i(int32_t x, int32_t y, int32_t z) : Vec3{x, y, z} {}

		Vector3i operator+(Vector3i other) const {
			return Vector3i(x + other.x, y + other.y, z + other.z);
		}

		Vector3i operator-(Vector3i other) const {
			return Vector3i(x - other.x, y - other.y, z - other.z);
		}

		Vector3i operator/(int r) const {
			return Vector3i(x / r, y / r, z / r);
		}

		Vector3i operator*(int r) const {
			return Vector3i(x * r, y * r, z * r);
		}
	};

	enum class VectorFormatStyle {
		Luau,
		CXX,
	};

	inline std::string to_string(const Vec2<float>& vec) {
		return std::format("{:g}, {:g}", vec.x, vec.y);
	}

	inline std::string to_string(const Vec3<float>& vec) {
		return std::format("{:g}, {:g}, {:g}", vec.x, vec.y, vec.z);
	}

	inline std::string to_string(const Vec2<uint32_t>& vec) {
		return std::format("{:d}u, {:d}u", vec.x, vec.y);
	}

	inline std::string to_string(const Vec2<int32_t>& vec) {
		return std::format("{:d}, {:d}", vec.x, vec.y);
	}

	inline std::string to_string(const Vec3<uint32_t>& vec) {
		return std::format("{:d}, {:d}, {:d}", vec.x, vec.y, vec.z);
	}

	inline std::string to_string(const Vec3<int32_t>& vec) {
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

	inline Vector2 clamp(const Vector2& value, const Vector2& min, const Vector2& max) {
		return Vector2(clamp(value.x, min.x, max.x), clamp(value.y, min.y, max.y));
	}

	inline Vector3 clamp(const Vector3& value, const Vector3& min, const Vector3& max) {
		return Vector3(clamp(value.x, min.x, max.x), clamp(value.y, min.y, max.y), clamp(value.z, min.z, max.z));
	}
} // namespace andromeda


namespace andromeda_luau {
	void luaL_pushVector2(lua_State* L, const andromeda::Vector2& value);
	andromeda::Vector2* luaL_toVector2(lua_State* L, int idx);
} // namespace andromeda_luau