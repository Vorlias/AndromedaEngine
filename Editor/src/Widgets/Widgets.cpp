#include "Widgets.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace andromeda::widgets {
	enum Axis {
		VECTOR_X,
		VECTOR_Y,
		VECTOR_Z,
	};

	static bool DrawAxisInput(Axis axis, float* value, float resetValue, ImVec2 buttonSize, NumberFormat format) {
		bool modified = false;
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[2];

		const char* id;
		const char* label;

		ImVec4 buttonColor;
		ImVec4 buttonHoveredColor;
		ImVec4 buttonActiveColor;

		switch (axis) {
			case VECTOR_X: {
				buttonColor = {0.8f, 0.1f, 0.15f, 1.0f};
				buttonHoveredColor = {0.9f, 0.2f, 0.2f, 1.0f};
				buttonActiveColor = {0.8f, 0.1f, 0.15f, 1.0f};
				label = "X";
				id = "##X";
				break;
			}
			case VECTOR_Y: {
				buttonColor = {0.2f, 0.7f, 0.3f, 1.0f};
				buttonHoveredColor = {0.3f, 0.8f, 0.3f, 1.0f};
				buttonActiveColor = {0.2f, 0.7f, 0.2f, 1.0f};
				label = "Y";
				id = "##Y";
				break;
			}
			case VECTOR_Z: {
				buttonColor = {0.1f, 0.25f, 0.8f, 1.0f};
				buttonHoveredColor = {0.3f, 0.5f, 0.9f, 1.0f};
				buttonActiveColor = {0.1f, 0.25f, 0.8f, 1.0f};
				label = "Z";
				id = "##Z";
				break;
			}
			default:
				return false;
		}

		// ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
		// ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
		ImGui::PushStyleColor(ImGuiCol_Text, buttonColor);
		ImGui::PushFont(boldFont);
		if (ImGui::Button(label, buttonSize)) {
			*value = resetValue;
			modified = true;
		}
		// ImGui::PopStyleColor(3);
		ImGui::PopStyleColor(1);
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::PopStyleColor();
		ImGui::SameLine();

		if (format == FORMAT_DEGREES) {
			if (ImGui::DragFloat(id, value, 0.1f, 0.0f, 0.0f, "%.1f°"))
				modified = true;
		} else {
			if (ImGui::DragFloat(id, value, 0.1f, 0.0f, 0.0f, "%.1f"))
				modified = true;
		}

		return modified;
	}

	bool Vector3F(Vector3& values, float resetValue, float columnWidth, NumberFormat format) {
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[1];
		bool modified = false;

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		{
			// float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			// float lineHeight = ImGui::GetTextLineHeightWithSpacing();

			float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;

			ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

			if (DrawAxisInput(VECTOR_X, &values.x, resetValue, buttonSize, format))
				modified = true;
			ImGui::PopItemWidth(); // 1

			ImGui::SameLine(0.0f, 3.0f);
			if (DrawAxisInput(VECTOR_Y, &values.y, resetValue, buttonSize, format))
				modified = true;
			ImGui::PopItemWidth(); // 2

			ImGui::SameLine(0.0f, 3.0f);
			if (DrawAxisInput(VECTOR_Z, &values.z, resetValue, buttonSize, format))
				modified = true;
			ImGui::PopItemWidth(); // 3
		}
		ImGui::PopStyleVar();

		return modified;
	}

	bool Vector3F(const std::string& label, Vector3& values, float resetValue, float columnWidth) {
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[1];

		bool modified = false;

		ImGui::PushID(label.c_str());
		ImGui::Columns(2, NULL, false);
		{
			ImGui::SetColumnWidth(0, columnWidth);

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 2));
			ImGui::AlignTextToFramePadding();
			ImGui::Text(label.c_str());
			ImGui::PopStyleVar();

			ImGui::NextColumn();

			modified = Vector3F(values, resetValue, columnWidth);
		}
		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}

	bool EulerAngles(const std::string& label, Vector3& values, float resetValue, float columnWidth) {
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[1];

		bool modified = false;

		ImGui::PushID(label.c_str());
		ImGui::Columns(2, NULL, false);
		{
			ImGui::SetColumnWidth(0, columnWidth);

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 2));
			ImGui::AlignTextToFramePadding();
			ImGui::Text(label.c_str());
			ImGui::PopStyleVar();

			ImGui::NextColumn();

			modified = Vector3F(values, resetValue, columnWidth, FORMAT_DEGREES);
		}
		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}

	void OffsetY(float yOffset) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		window->DC.CurrLineTextBaseOffset = ImMax(window->DC.CurrLineTextBaseOffset, yOffset);
	}

	internal::ObjectContainerAction ObjectContainer(const char* label, bool hasObject, std::string_view objName, std::string_view clsName) {
		using namespace internal;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return OBJECT_ACTION_NONE;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		float w = ImGui::GetColumnWidth();

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImVec2(w, window->DC.CurrLineSize.y);

		const ImRect fieldBB(pos, ImVec2(pos.x + size.x - 31, pos.y + size.y));
		const ImRect pickerBB(pos + ImVec2(size.x - 30, 0), ImVec2(pos.x + size.x, pos.y + size.y));

		ImGui::ItemSize(fieldBB.GetSize(), style.FramePadding.y);

		if (!ImGui::ItemAdd(fieldBB, id))
			return OBJECT_ACTION_NONE;

		auto bg = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
		auto bgHover = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));

		auto btn = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Button));
		auto btnHover = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));

		auto mouseOverField = ImGui::IsMouseHoveringRect(fieldBB.Min, fieldBB.Max);
		auto mouseOverPicker = ImGui::IsMouseHoveringRect(pickerBB.Min, pickerBB.Max);

		if (mouseOverField) {
			window->DrawList->AddRectFilled(fieldBB.Min, fieldBB.Max, bgHover);
		} else {
			window->DrawList->AddRectFilled(fieldBB.Min, fieldBB.Max, bg);
		}

		window->DrawList->AddRectFilled(pickerBB.Min, pickerBB.Max, mouseOverPicker ? btnHover : btn);
		window->DrawList->AddText(
			g.Font, g.FontSize, pickerBB.Min + ImVec2(8.0f, 8.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), "" ICON_LC_CIRCLE_DOT, 0, 0
		);

		if (hasObject) {
			window->DrawList->AddText(g.Font, g.FontSize, fieldBB.Min + style.FramePadding, ImGui::GetColorU32(ImGuiCol_Text), objName.data(), 0, 0);
		} else {
			window->DrawList->AddText(
				g.Font, g.FontSize, fieldBB.Min + style.FramePadding, ImGui::GetColorU32(ImGuiCol_TextDisabled), "(None)", 0, 0
			);
		}

		if (mouseOverField) {
			if (ImGui::IsMouseDoubleClicked(0)) {
				return OBJECT_ACTION_DBLCLICK;
			}

			if (ImGui::IsMouseClicked(1)) {
				return OBJECT_ACTION_CONTEXT;
			}

			if (ImGui::IsMouseClicked(0)) {
				return OBJECT_ACTION_CLICK;
			}
		} else if (mouseOverPicker) {
			if (ImGui::IsMouseClicked(0)) {
				return OBJECT_ACTION_SELECT;
			}
		}

		return OBJECT_ACTION_NONE;
	}
} // namespace andromeda::widgets