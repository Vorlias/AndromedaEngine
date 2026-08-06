#include "Engine/Data/Vector.h"
#include <math.h>

static float signf(float value) {
    if (value >= 0) return 1;
    return -1;
}

namespace andromeda {
    float Vector2::GetMagnitude() {
        return sqrt(x * x + y * y);
    }
    
    float Vector2::GetDistance(Vector2 other) {
        float diffX = x - other.x;
        float diffY = y - other.y;
        return sqrt(diffX * diffX + diffY * diffY);
    }

    Vector3 Vector2::Extend(float z) {
        return Vector3(x, y, z);
    }

    float Vector3::GetMagnitude() {
        return sqrt(x * x + y * y + z * z);
    }

    float Vector3::GetDistance(Vector3 other) {
        float diffX = x - other.x;
        float diffY = y - other.y;
        float diffZ = z - other.z;
        return sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ);
    }

    Vector2 Vector3::Shrink() {
        return Vector2(x, y);
    }
}