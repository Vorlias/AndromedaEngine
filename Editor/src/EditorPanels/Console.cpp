#include "Console.h"
#include "imgui/imgui.h"



void andromeda::Console::Draw() {
	ImGuiWindowFlags windowFlags = 0;
	bool overlay = consoleFlags & ConsoleFlags_DisplayAsOverlay;

	if (overlay) {
		static int location = 2;

		windowFlags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize |
		               ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		ImGui::SetNextWindowSize(ImVec2(500, 200));

		windowFlags |= ImGuiWindowFlags_NoMove;
	} else {
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	}

	if (!ImGui::Begin(overlay ? "Console##overlay" : "Console##console", &open, windowFlags)) {
		ImGui::End();
		return;
	}

	if (font != nullptr)
		ImGui::PushFont(font);
	{
		if (ImGui::BeginChild("ScrollingRegion", ImVec2(), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar)) {
			for (auto& item : items) {
				ImVec4 color;
				bool has_color = false;

				switch (item.level) {
					case spdlog::level::info: {
						color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);
						has_color = true;
						break;
					}
					case spdlog::level::warn: {
						color = ImVec4(1.0f, 1.0f, 0.5f, 1.0f);
						has_color = true;
						break;
					}
					case spdlog::level::err: {
						color = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
						has_color = true;
						break;
					}
				}

				if (has_color)
					ImGui::PushStyleColor(ImGuiCol_Text, color);

				ImGui::TextUnformatted(item.message.c_str());

				if (has_color)
					ImGui::PopStyleColor();
			}


			if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
				ImGui::SetScrollHereY(1.0f);

			scrollToBottom = false;
		}
		ImGui::EndChild();
	}
	if (font != nullptr)
		ImGui::PopFont();

	ImGui::End();
}