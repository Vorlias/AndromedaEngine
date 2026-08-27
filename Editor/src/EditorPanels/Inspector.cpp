#include "Inspector.h"
#include "imgui/imgui.h"
#include "../Widgets/IconsLucide.h"
#include "../Widgets/Widgets.h"
#include <string.h>

#define ITEM_ADD(T, name) \
	if (!selected.HasComponent<T>()) \
		if (ImGui::MenuItem(name)) { \
			selected.AddComponent<T>(); \
		}

template<typename T>
using ComponentRender = void(T&);

namespace andromeda {
	InspectorPanel::InspectorPanel(const SharedRef<Scene>& context) {
		SetContext(context);
	}

	void InspectorPanel::SetContext(const SharedRef<Scene>& context) {
		this->context = context;
		this->selected = Entity();
	}

	void InspectorPanel::SetActiveEntity(const Entity entity) {
		this->selected = entity;
	}

	void InspectorPanel::DrawInspector() {
		ImGui::SetNextWindowSizeConstraints(ImVec2(370.0f, 200.0f), ImVec2(1000, 1000));
		ImGui::Begin(ICON_LC_INFO " Inspector###inspector");
		{
			// static GUID testguid = CreateGUID();
			// ImGui::Text("%s", testguid.str().c_str());

			if (selected) {
				DrawComponents(selected);

				auto avail = ImGui::GetWindowWidth();

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::Spacing();
				ImGui::SameLine(avail / 2 - (200.0f / 2));
				if (ImGui::Button("Add Component", ImVec2(200.0f, 30.0f))) {
					ImGui::OpenPopup("AddComponent");
				}
				ImGui::SameLine();
				ImGui::Spacing();

				if (ImGui::BeginPopup("AddComponent")) {
					ITEM_ADD(LuauScriptComponent, "Luau Script");
					// ITEM_ADD(SpriteRendererComponent, "Sprite Renderer");
					// if (!selected.HasComponent<LuauScriptComponent>()) {
					// 	if (ImGui::MenuItem("Luau Script")) {
					// 		auto& component = selected.AddComponent<LuauScriptComponent>();
					// 	};
					// }
					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();
	}

	template<typename T>
	void DrawComponent(Entity& entity, ComponentRender<T> drawFn, const std::string& heading) {
		ANDROMEDA_ASSERTM(entity.HasComponent<T>(), "Invalid");

		if (ImGui::CollapsingHeader(heading.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			auto& component = entity.GetComponent<T>();
			drawFn(component);
		}
	}

	template<typename T>
	void DrawComponentIfExists(Entity& entity, ComponentRender<T> drawFn, const std::string& heading) {
		if (!entity.HasComponent<T>())
			return;
		ImGui::PushID(heading.c_str());

		bool removeComponent = false;

		bool open = ImGui::CollapsingHeader(heading.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
		ImGui::SameLine(ImGui::GetColumnWidth() - 10.0f);

		if (ImGui::Button(ICON_LC_MENU, ImVec2(30, 30))) {
			ImGui::OpenPopup("ComponentSettings");
		}

		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Remove Component")) {
				removeComponent = true;
			}

			ImGui::EndPopup();
		}

		if (open) {
			auto& component = entity.GetComponent<T>();
			drawFn(component);
		}

		ImGui::PopID();
		if (removeComponent)
			entity.RemoveComponent<T>();
	}

	void InspectorPanel::DrawComponents(Entity entity) {
		DrawComponent<TransformComponent>(
			entity,
			[](TransformComponent& transform) {
				widgets::Vector3F(ICON_LC_MOVE_3D "\tPosition", transform.position);

				Vector3 rotation = radiansToDegrees(transform.rotation);
				if (widgets::EulerAngles(ICON_LC_ROTATE_3D "\tRotation", rotation)) {
					transform.rotation = degreesToRadians(rotation);
				}

				widgets::Vector3F(ICON_LC_SCALE_3D "\tScale", transform.scale, 1.0f);
			},
			"Transform"
		);

		DrawComponentIfExists<LuauScriptComponent>(
			entity,
			[](LuauScriptComponent& component) {
				// char buffer[256];
				// memset(buffer, 0, sizeof(buffer));

				// auto script = component.GetScript();
				// auto scriptPath = script->GetFilePath();

				// strcpy(buffer, scriptPath.c_str());

				// if (ImGui::InputText("Script Path", buffer, sizeof(buffer))) {}
			},
			"Luau Script"
		);
	}
} // namespace andromeda