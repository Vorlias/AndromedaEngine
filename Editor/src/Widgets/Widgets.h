#pragma once
#include <string>
#include "../Widgets/IconsLucide.h"


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <Andromeda.h>

namespace andromeda::widgets {
	// bool Vec2(glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	enum NumberFormat {
		FORMAT_DECIMAL,
		FORMAT_DEGREES,
	};

	namespace internal {
		enum ObjectContainerAction {
			OBJECT_ACTION_NONE,
			OBJECT_ACTION_CLICK,
			OBJECT_ACTION_DBLCLICK,
			OBJECT_ACTION_CONTEXT,

			OBJECT_ACTION_SELECT,
		};
	}

	bool Vector3F(Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f, NumberFormat format = NumberFormat::FORMAT_DECIMAL);
	bool Vector3F(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	bool EulerAngles(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	internal::ObjectContainerAction ObjectContainer(const char* id, bool hasObject, std::string_view objName, std::string_view clsName);

	void OffsetY(float yOffset);

	enum DrawTextPreviousLineFlags {
		TextPreviousLine_None = 0,
		TextPreviousLine_VerticalOnly = 1,
	};

	void DrawTextPreviousLine(
		const char* icon,
		ImVec2 offset,
		ImVec4 col = ImVec4(1, 1, 1, 1),
		DrawTextPreviousLineFlags flags = TextPreviousLine_None
	);
	ImVec2 GetPadding();
	int GetTreeDepth();

	void DrawText(ImVec2 pos, const char* txt, ImVec4 col);

	template<typename T>
	Ref<T> Object(const std::string& label, Ref<T> value) {
		if constexpr (std::is_base_of_v<Asset, T>) {
			ImGui::AlignTextToFramePadding();
			ImGui::Text(label.c_str());
			ImGui::SameLine();

			if constexpr (std::is_base_of_v<LuauScript, T>) {
				OffsetY(9);
				ImGui::Text(ICON_LC_SCROLL);
				ImGui::SameLine();
			}

			AssetType assetType = value != nullptr ? value->GetAssetType() : AssetType::Unknown;

			int result =
				ObjectContainer(label.c_str(), value != nullptr, value != nullptr ? value->GetFilePath() : "", GetAssetTypeName(assetType).c_str());
			if (result) {
				print("Picker selection is {}", result);
			}
		}

		return value;
	}
} // namespace andromeda::widgets