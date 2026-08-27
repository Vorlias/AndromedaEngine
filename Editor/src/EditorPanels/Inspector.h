#pragma once
#include <Andromeda.h>

namespace andromeda {
	class InspectorPanel {
	public:
		InspectorPanel(const SharedRef<Scene>& context);
		void SetContext(const SharedRef<Scene>& context);
		void SetActiveEntity(Entity entity);
		void DrawInspector();

	private:
		void DrawComponents(Entity entity);

	private:
		SharedRef<Scene> context;
		Entity selected;
	};
} // namespace andromeda