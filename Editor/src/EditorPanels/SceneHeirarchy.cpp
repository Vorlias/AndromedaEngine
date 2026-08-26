#include "../Widgets/IconsLucide.h"
#include "SceneHeirarchy.h"
#include "imgui/imgui.h"

#include "Engine/Objects/Component.h"

andromeda::SceneHierarchyPanel::SceneHierarchyPanel(andromeda::SharedRef<Scene> scene) : m_scene(scene) {}

void andromeda::SceneHierarchyPanel::SetContext(const SharedRef<Scene>& scene) {
	m_scene = scene;
}

void andromeda::SceneHierarchyPanel::DrawEntityNode(andromeda::Entity entity) {
	bool destroy = false;
	auto& name = entity.GetComponent<NameComponent>().name;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (m_selected == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}


	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
	ImGui::PushID(entity.GetId());
	if (ImGui::TreeNodeEx(*entity, flags, ICON_LC_BOX "\t%s", name.c_str())) {
		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopStyleVar();

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

	if (destroy) {
		if (m_selected == entity)
			onSelect(Entity());
		m_scene->DestroyEntity(entity);
	}
}

void andromeda::SceneHierarchyPanel::DrawHierarchyPanel() {
	
	ImGui::Begin(ICON_LC_LIST_TREE " Hierarchy##sceneHierarchy");
	{
		if (m_scene) {
			auto& registry = m_scene->GetRegistry();
			auto entities = registry.view<andromeda::NameComponent, andromeda::TransformComponent>();

			for (auto [entityId, name, transform] : entities.each()) {
				Entity entity(m_scene.get(), entityId);
				DrawEntityNode(entity);
			}
		}
	}
	ImGui::End();
}