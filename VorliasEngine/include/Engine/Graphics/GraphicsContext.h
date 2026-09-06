#pragma once
#include "Engine/Graphics/RenderCommand.h"
#include "Engine/Graphics/RenderTarget.h"
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

	enum class API;

	class GraphicsContext {
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void Resize(int width, int height) = 0;

		virtual void BeforeRender() {}
		virtual void RenderPrepare() {}
		virtual void RenderDraw() {}
		virtual void RenderPresent() {}

		virtual API GetAPI() = 0;

		template<typename T, typename... Args>
			requires(std::is_base_of_v<andromeda::graphics::RenderCommand, T> && !std::is_same_v<andromeda::graphics::RenderCommand, T>)
		void Submit(Args&&... args) {
			std::unique_ptr<andromeda::graphics::RenderCommand> command = createCommand<T>(std::forward<Args>(args)...);
			SubmitCommand(std::move(command));
		}

		virtual void SubmitCommand(std::unique_ptr<andromeda::graphics::RenderCommand> command) {}

		virtual void SetClearColor(Color color) {}

		// Set the render target of this graphics renderer
		virtual void SetRenderTarget(std::shared_ptr<RenderTexture> renderTarget) = 0;

		friend class RenderCommand;
	};
} // namespace andromeda::graphics