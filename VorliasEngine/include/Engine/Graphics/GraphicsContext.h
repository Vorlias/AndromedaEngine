#pragma once
#include "Engine/Data/Color.h"
#include "Engine/Data/Rect.h"
#include <vector>

namespace andromeda::graphics {
	// struct Drawable {
	// 	virtual void Draw() = 0;
	// };

	// struct DrawData {
	// 	std::vector<int> indicies;
	// 	std::vector<Vertex> vertices;
	// };

	// struct RenderData {
	// 	Rect viewport;
	// 	Rect viewportScissor;
	// 	std::vector<DrawData> drawList;
	// };

	class GraphicsContext {
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void Resized(int width, int height) = 0;

		virtual void BeforeRender() {}
		virtual void RenderPrepare() {}
		virtual void RenderDraw() {}
		virtual void RenderPresent() {}

		virtual void SetClearColor(Color color) {}

		friend class RenderCommand;
	};
} // namespace andromeda::graphics