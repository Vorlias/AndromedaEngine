#pragma once
namespace andromeda::graphics {
	class GraphicsContext {
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void Resized(int width, int height) = 0;
	};
} // namespace andromeda::graphics