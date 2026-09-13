#pragma once
#include "Vector.h"
#include "Engine/Math.h"
#include "lualib.h"
#include <format>
#include <ostream>

namespace andromeda {
	struct Rect;
	// Represents an integer-based 2D rectangle
	struct IntRect {
		Vector2i size;
		Vector2i position;

		IntRect() = default;
		IntRect(Vector2i position, Vector2i size) : size(size), position(position) {}
		IntRect(int32_t left, int32_t top, int32_t width, int32_t height) : size(width, height), position(left, top) {}

		[[nodiscard]] constexpr int32_t GetX() const {
			return position.x;
		}
		[[nodiscard]] constexpr int32_t GetY() const {
			return position.y;
		}
		[[nodiscard]] constexpr int32_t GetWidth() const {
			return size.x;
		}
		[[nodiscard]] constexpr int32_t GetHeight() const {
			return size.y;
		}

		[[nodiscard]] const Vector2i GetCenter() const {
			return position + (size / 2);
		}
	};

	// Represents a 2D rectangle
	struct Rect {
	public:
		static constexpr uint32_t elements = Vector2::elements * 2;
		static constexpr size_t bytes = elements * sizeof(float);

		Rect() = default;
		Rect(Vector2 position, Vector2 size) : size(size), position(position) {}
		Rect(float left, float top, float width, float height) : size(width, height), position(left, top) {}

		Rect(const IntRect& intRect)
			: size(static_cast<float>(intRect.size.x), static_cast<float>(intRect.size.y))
			, position(static_cast<float>(intRect.position.x), static_cast<float>(intRect.position.y)) {}

		Vector2 size;
		Vector2 position;

		[[nodiscard]] constexpr float GetX() const {
			return position.x;
		}
		[[nodiscard]] constexpr float GetY() const {
			return position.y;
		}
		[[nodiscard]] constexpr float GetWidth() const {
			return size.x;
		}
		[[nodiscard]] constexpr float GetHeight() const {
			return size.y;
		}

		[[nodiscard]] constexpr Vector2 GetMin() const {
			const float minX = minf(position.x, position.x + size.x);
			const float minY = minf(position.y, position.y + size.y);
			return Vector2(minX, minY);
		}

		[[nodiscard]] constexpr Vector2 GetMax() const {
			const float maxX = maxf(position.x, position.x + size.x);
			const float maxY = maxf(position.y, position.y + size.y);
			return Vector2(maxX, maxY);
		}

		[[nodiscard]] bool Contains(const Vector2& point) const {
			const Vector2 min = GetMin();
			const Vector2 max = GetMax();

			return (point.x >= min.x) && (point.x < max.x) && (point.y >= min.y) && (point.y < max.y);
		}

		[[nodiscard]] const Vector2 GetCenter() const {
			return position + (size / 2.f);
		}

		Vector2 GetNormalizedVector(Vector2 point) const {
			return Vector2((point.x - position.x) / size.x, (point.y - position.y) / size.y);
		}

		Vector2 GetAbsoluteVector(Vector2 normal) {
			return Vector2((normal.x * size.x) + position.x, (normal.y * size.y) + position.y);
		}

		inline bool operator==(Rect& r) {
			return r.size == size && r.position == position;
		}
	};

	// Represents a 3D bounding box - essentially a reactangular prism
	struct Bounds {
		static constexpr uint32_t elements = Vector3::elements * 2;
		static constexpr size_t bytes = elements * sizeof(float);

		Vector3 size{};
		Vector3 center{};

		Bounds() = default;
		Bounds(Vector3 center, Vector3 size) : size(size), center(center) {}

		// Grow the bounds to include the given point
		void Encapsulate(const Vector3& point) {
			Vector3 mmin = Vector3::Min(GetMin(), point);
			Vector3 mmax = Vector3::Max(GetMax(), point);
			SetMinMax(mmin, mmax);
		}

		// Sets the bounds to the min and max value of the box
		void SetMinMax(const Vector3& min, const Vector3& max) {
			size.x = max.x - min.x;
			size.y = max.y - min.y;
			size.z = max.z - min.z;

			center.x = min.x + size.x / 2.0f;
			center.y = min.y + size.y / 2.0f;
			center.z = min.z + size.z / 2.0f;
		}

		bool Intersects(const Vector3& other) {
			Vector3 min = GetMin();
			Vector3 max = GetMax();
			return (other.x >= min.x) && (other.y >= min.y) && (other.z >= min.z) && (other.x <= max.x) && (other.y <= max.y) && (other.z <= max.z);
		}

		[[nodiscard]] Vector3 GetMin() const {
			return center - size * 0.5f;
		}

		[[nodiscard]] Vector3 GetMax() const {
			return center + size * 0.5f;
		}

		[[nodiscard]] Vector3 GetExtents() const {
			return size * 0.5f;
		}

		[[nodiscard]] Vector3 GetSize() const {
			return size;
		}

		[[nodiscard]] Vector3 GetCenter() const {
			return center;
		}

		[[nodiscard]] bool Contains(const Vector3& point) const {
			const Vector3 min = GetMin();
			const Vector3 max = GetMax();

			return (point.x >= min.x) && (point.x < max.x) && (point.y >= min.y) && (point.y < max.y) && (point.z >= min.z) && (point.z < max.z);
		}

		Bounds& Expand(float amount) {
			size.x += amount;
			size.y += amount;
			size.z += amount;
			return *this;
		}

		Bounds& Expand(Vector3 amount) {
			size.x += amount.x;
			size.y += amount.y;
			size.z += amount.z;
			return *this;
		}

		inline bool operator==(Bounds& r) {
			return size == r.size && center == r.center;
		}
	};

	struct IntBounds {
		Vector3i size{};
		Vector3i center{};

		IntBounds() = default;
		IntBounds(Vector3i center, Vector3i size) : size(size), center(center) {}
	};

	inline std::string to_string(const Bounds& rect) {
		return "(center: " + to_string(rect.center) + "; size: " + to_string(rect.size) + ")";
	}

	inline std::string to_string(const IntBounds& rect) {
		return "(center: " + to_string(rect.center) + "; size: " + to_string(rect.size) + ")";
	}

	inline std::string to_string(const Rect& rect) {
		return std::format("(x: {:g}, y: {:g}, width: {:g}, height: {:g})", rect.position.x, rect.position.y, rect.size.x, rect.size.y);
	}

	inline std::string to_string(const IntRect& rect) {
		return std::format("(x: {:d}, y: {:d}, width: {:d}, height: {:d})", rect.position.x, rect.position.y, rect.size.x, rect.size.y);
	}

	inline Vector2 clamp(const Vector2& value, const Rect& rect) {
		return clamp(value, rect.position, rect.position + rect.size);
	}

	inline Vector3 clamp(const Vector3& value, const Bounds& bounds) {
		return clamp(value, bounds.GetMin(), bounds.GetMax());
	}
} // namespace andromeda

namespace andromeda_luau {
	void registerRectLib(lua_State* L);
	inline void registerBoundsLib(lua_State* L) {}

	andromeda::Rect* luaL_newRect(lua_State* L);
	andromeda::Rect* luaL_pushRect(lua_State* L, const andromeda::Vector2& position, const andromeda::Vector2& size);
	andromeda::Rect* luaL_pushRect(lua_State* L, float left, float top, float width, float height);
}

std::ostream& operator<<(std::ostream& stream, const andromeda::Vector2& vec);
std::ostream& operator<<(std::ostream& stream, const andromeda::Vector3& vec);

std::ostream& operator<<(std::ostream& stream, const andromeda::Rect& rect);
std::ostream& operator<<(std::ostream& stream, const andromeda::IntRect& rect);

std::ostream& operator<<(std::ostream& stream, const andromeda::Bounds& rect);
std::ostream& operator<<(std::ostream& stream, const andromeda::IntBounds& rect);