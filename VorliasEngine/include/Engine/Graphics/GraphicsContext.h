#pragma once
#include "Engine/Data/Color.h"

namespace andromeda::graphics {
	class GraphicsContext {
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void Resized(int width, int height) = 0;

		virtual void Prepare() {}
		virtual void Render() {}
		virtual void Present() {}

		virtual void SetClearColor(Color color) {}

		friend class RenderCommand;
	};
} // namespace andromeda::graphics