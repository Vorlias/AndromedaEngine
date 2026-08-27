#pragma once
#include <Andromeda.h>

namespace andromeda {
	class SceneHierarchyPanel {
	public:
		using SelectionCallback = std::function<bool(Entity)>; // bool(Entity entity);

		SceneHierarchyPanel(SharedRef<Scene> scene);
		void SetContext(const SharedRef<Scene>& scene);
		void DrawHierarchyPanel();

        SelectionCallback onSelect;
	private:
		void DrawEntityNode(Entity entity, const andromeda::EntityRelationships& relationships);

	private:
		SharedRef<Scene> m_scene;
		Entity m_selected;
	};
} // namespace andromeda