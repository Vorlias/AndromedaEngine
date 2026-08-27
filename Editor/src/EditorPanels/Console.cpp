#include "Console.h"
#include "imgui/imgui.h"



void andromeda::Console::Draw() {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Console##console", &open)) {
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