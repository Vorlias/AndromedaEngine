#pragma once
#include <string>
// #include <external/glm/glm.hpp>
// #include <Andromeda/scene/components.h>
// #include <Andromeda/uuid.h>
// #include <Andromeda/assets.h>
#include <Andromeda.h>

namespace andromeda::widgets {
	// bool Vec2(glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);

    enum NumberFormat {
        FORMAT_DECIMAL,
        FORMAT_DEGREES,
    };

	bool Vector3F(Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f, NumberFormat format = NumberFormat::FORMAT_DECIMAL);
	
    bool Vector3F(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
    bool EulerAngles(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
}