#include "Engine/Data/Vector.h"
#include <math.h>
#include "lualib.h"

static float signf(float value) {
	if (value >= 0)
		return 1;
	return -1;
}


namespace andromeda {
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

