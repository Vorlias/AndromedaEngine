#include "Engine/Data/Vector.h"
#include <math.h>
#include "lualib.h"

static float signf(float value) {
	if (value >= 0)
		return 1;
	return -1;
}


namespace andromeda {
	Vector2 Vector2::zero = Vector2();
	Vector2 Vector2::one = Vector2(1, 1);
	Vector2 Vector2::left = Vector2(-1, 0);
	Vector2 Vector2::right = Vector2(1, 0);
	Vector2 Vector2::up = Vector2(0, 1);
	Vector2 Vector2::down = Vector2(0, -1);

	Vector3 Vector3::zero = Vector3();
	Vector3 Vector3::one = Vector3(1, 1, 1);
	Vector3 Vector3::left = Vector3(-1, 0, 0);
	Vector3 Vector3::right = Vector3(1, 0, 0);
	Vector3 Vector3::up = Vector3(0, 1, 0);
	Vector3 Vector3::down = Vector3(0, -1, 0);
	Vector3 Vector3::forward = Vector3(0, 0, 1);
	Vector3 Vector3::backward = Vector3(0, 0, -1);

	float Vector2::GetMagnitude() const {
		return sqrt(x * x + y * y);
	}

	Vector2 Vector2::Normalize() const {
		auto mag = GetMagnitude();
		if (mag > kEpsilon) {
			return Vector2(x / mag, y / mag);
		} else {
			return Vector2();
		}
	}

	float Vector2::GetDistance(Vector2 other) const {
		float diffX = x - other.x;
		float diffY = y - other.y;
		return sqrt(diffX * diffX + diffY * diffY);
	}

	Vector3 Vector2::Extend(float z) {
		return Vector3(x, y, z);
	}

	float Vector3::GetMagnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 Vector3::Normalize() const {
		auto mag = GetMagnitude();
		if (mag > kEpsilon) {
			return Vector3(x / mag, y / mag, z / mag);
		} else {
			return Vector3();
		}
	}

	float Vector3::GetDistance(Vector3 other) const {
		float diffX = x - other.x;
		float diffY = y - other.y;
		float diffZ = z - other.z;
		return sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ);
	}

	Vector2 Vector3::Shrink() {
		return Vector2(x, y);
	}
} // namespace andromeda

