#pragma once
#include "GraphicsContext.h"

namespace andromeda::graphics {
	class RenderCommand {
	public:
		virtual void Execute(GraphicsContext* context) = 0;
		virtual ~RenderCommand() {}
	};

	class RenderClearColorCommand final : public RenderCommand {
		RenderClearColorCommand(const Color& color) : color(color) {}
		void Execute(GraphicsContext* context) override {
			context->SetClearColor(color);
		}

	private:
		Color color;
	};
} // namespace andromeda::graphics