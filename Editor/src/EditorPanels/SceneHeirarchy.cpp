#include "../Widgets/IconsLucide.h"
#include "SceneHeirarchy.h"
#include "imgui/imgui.h"
#include "../IMGUIExt.h"
#include "../Widgets/Widgets.h"

#include "Engine/Objects/Component.h"

andromeda::SceneHierarchyPanel::SceneHierarchyPanel(andromeda::SharedRef<Scene> scene) : m_scene(scene) {}

void andromeda::SceneHierarchyPanel::SetContext(const SharedRef<Scene>& scene) {
	m_scene = scene;
}

void andromeda::SceneHierarchyPanel::DrawEntityNode(andromeda::Entity entity, const andromeda::EntityRelationships& rel, int level) {
	bool destroy = false;
	auto& name = entity.GetComponent<NameComponent>().name;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (rel.children.size() == 0) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DrawLinesToNodes;
	} else {
		flags |= ImGuiTreeNodeFlags_DrawLinesToNodes;
	}

	if (m_selected == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 1.0f));

	ImGui::PushID(entity.GetId());
	auto id = ImGui::GetID(entity.GetId());


	bool open = ImGui::TreeNodeEx(*entity, flags, "%s", name.c_str());
	auto cursor = ImGui::GetCursorPos();

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Delete")) {
			destroy = true;
		}

		ImGui::EndPopup();
	}

	if (ImGui::IsItemClicked() && onSelect) {
		if (onSelect(entity))
			m_selected = entity;
	}

	// ImGui::AlignTextToFramePadding();

	if (rel.children.size() == 0) {
		widgets::DrawText(cursor + ImVec2(-13, 8), ICON_LC_BOX, ImVec4(1, 1, 1, 1));
	}

	float offset = 0.0f;



	if (entity.HasComponent<LuauScriptComponent>()) {
		widgets::DrawTextPreviousLine(ICON_LC_SCROLL, ImVec2(8, 8), ImVec4(0.2, 0.2, 0.2, 1));
		offset += 20.0f;
	}

	if (entity.HasComponent<CameraComponent>()) {
		// ImGui::AlignTextToFramePadding();
		// ImGui::SameLine(ImGui::GetColumnWidth() - offset);
		// ImGui::Text(ICON_LC_CAMERA);
	}

	if (destroy) {
		if (m_selected == entity)
			onSelect(Entity());
		m_scene->DestroyEntity(entity);
	}

	if (open) {
		if (rel.children.size() > 0 && !destroy) {
			auto& registry = m_scene->GetRegistry();

			for (auto child : rel.children) {
				Entity childEntity(m_scene.get(), child);
				auto& childRel = registry.get<EntityRelationships>(child);
				DrawEntityNode(childEntity, childRel, level + 1);
			}
		}

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopStyleVar(2);
}

void andromeda::SceneHierarchyPanel::DrawHierarchyPanel() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
	ImGui::Begin(ICON_LC_LIST_TREE " Scene##sceneHierarchy");
	{
		if (m_scene) {
			auto& registry = m_scene->GetRegistry();
			auto entities = registry.view<andromeda::NameComponent, andromeda::EntityRelationships>();

			for (auto [entityId, name, rel] : entities.each()) {
				Entity entity(m_scene.get(), entityId);

				if (rel.parent == entt::null)
					DrawEntityNode(entity, rel);
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
				m_selected = Entity();
				onSelect(m_selected);
			}

			// Right clicking on the heirachy
			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
				if (ImGui::MenuItem("Create Entity")) {
					auto entity = m_scene->CreateEntity("Entity");
					onSelect(entity);
					m_selected = entity;
				}

				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}