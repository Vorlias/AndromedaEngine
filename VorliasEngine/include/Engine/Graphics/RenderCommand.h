#pragma once
#include "Engine/Data/Color.h"
#include "Engine/Graphics/Shader.h"

// #include "Engine/Graphics/GraphicsContext.h"
// #include "Engine/Graphics/RendererAPI.h"

namespace andromeda::graphics {
	class GraphicsContext;
	struct VertexArray;

	class RenderCommand {
	public:
		virtual ~RenderCommand() {}

		virtual void BeforeRender(GraphicsContext* ctx) {}

		virtual void Bind(GraphicsContext* ctx) {}
		virtual void Draw(GraphicsContext* ctx) {}
	};

	class ClearColorCommand : public RenderCommand {
	public:
		ClearColorCommand(const Color& color) : color(color) {}

		// void BeforeRender(GraphicsContext* ctx) {
		// 	ctx->SetClearColor(color);
		// }
	private:
		Color color;
	};

	class DrawVertexArrayCommand : public RenderCommand {
	public:
		DrawVertexArrayCommand(SharedRef<Shader> shader, const VertexArray& vertices, uint32_t instances)
			: m_shader(shader), m_vertices(vertices), m_instances(instances) {}

		void Draw(GraphicsContext* ctx) override;
	private:
		SharedRef<Shader> m_shader;
		const VertexArray& m_vertices;
		uint32_t m_instances;
	};

	class DrawVkTriangleDemoCommand : public RenderCommand {
	public:
		void Draw(GraphicsContext* context);
	};

	template<typename T, typename... Args>
		requires std::is_base_of_v<RenderCommand, T>
	std::unique_ptr<RenderCommand> createCommand(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
} // namespace andromeda::graphics