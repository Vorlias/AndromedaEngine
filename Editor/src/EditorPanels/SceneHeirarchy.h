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
		bool DrawEntityNode(Entity entity, const andromeda::EntityRelationships& relationships, int level = 0);

	private:
		SharedRef<Scene> m_scene;
		Entity m_selected;
		bool m_showHierarchyView = true;
	};
} // namespace andromeda