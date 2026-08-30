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

void andromeda::SceneHierarchyPanel::DrawEntityNode(andromeda::Entity entity, const andromeda::EntityRelationships& rel) {
	bool destroy = false;
	auto& name = entity.GetComponent<NameComponent>().name;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (rel.childCount == 0) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DrawLinesToNodes;
	} else {
		// flags |= ImGuiTreeNodeFlags_CollapsingHeader;
	}

	if (m_selected == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 1.0f));

	ImGui::PushID(entity.GetId());
	auto id = ImGui::GetID(entity.GetId());

	bool open = ImGui::TreeNodeEx(*entity, flags, "\t%s", name.c_str());
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

	ImGui::SameLine(ImGui::GetCursorStartPos().x + 15);
		widgets::OffsetY(8);
	ImGui::Text(ICON_LC_BOX);

	float offset = 0.0f;



	if (entity.HasComponent<LuauScriptComponent>()) {
		ImGui::AlignTextToFramePadding();
		ImGui::SameLine(ImGui::GetColumnWidth() - offset);
		ImGui::Text(ICON_LC_SCROLL);
		offset += 20.0f;
	}

	if (entity.HasComponent<CameraComponent>()) {
		ImGui::AlignTextToFramePadding();
		ImGui::SameLine(ImGui::GetColumnWidth() - offset);
		ImGui::Text(ICON_LC_CAMERA);
	}

	if (destroy) {
		if (m_selected == entity)
			onSelect(Entity());
		m_scene->DestroyEntity(entity);
	}

	if (open) {
		// if (rel.childCount > 0) {
		// 	auto& registry = m_scene->GetRegistry();

		// 	auto child = rel.firstChild;
		// 	for (std::size_t i = 0; i < rel.childCount; ++i) {
		// 		if (!registry.valid(child)) continue;

		// 		auto& childRel = registry.get<EntityRelationships>(child);
		// 		Entity childEntity(m_scene.get(), child);

		// 		DrawEntityNode(childEntity, childRel);
		// 		child = childRel.nextSibling;
		// 	}
		// }

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