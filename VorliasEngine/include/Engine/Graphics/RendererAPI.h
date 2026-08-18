#pragma once
#include "Engine/Data/Color.h"
#include "Engine/Data/Vector.h"
#include "Engine/Graphics/GraphicsContext.h"
#include <SDL3/SDL.h>

namespace andromeda::graphics {
	enum class API {
		None = 0,
		// Experimental Vulkan Support
		Vulkan = 1,
#ifdef ANDROMEDA_WGPU
		WGPU = 2,
#endif
#ifdef ANDROMEDA_OPENGL
		OpenGL = 3,
#endif
#ifdef ANDROMEDA_MAC
		Metal = 4,
#endif
#ifdef ANDROMEDA_WIN
		DirectX = 5,
#endif
	};

	// A collection of supported APIs in Andromeda
	constexpr API supportedAPIs[] = {
		API::None,
		API::Vulkan,
#ifdef ANDROMEDA_OPENGL
		API::OpenGL,
#endif
	};

	class Drawable {
	protected:
		friend class RenderSurface;
		virtual void Draw(GraphicsContext* ctx) = 0;
	};

	struct Vertex {
		Vector2 position;
		Color color{1, 1, 1};

		Vertex() = default;
		Vertex(Vector2 position) : position(position) {}
		Vertex(Vector2 position, Color color) : position(position), color(color) {}
	};

	struct VertexArray {
		VertexArray(std::vector<Vertex> vertices) {
			data = vertices.data();
			size = vertices.size();
		}

		template<std::size_t N>
		VertexArray(std::array<Vertex, N> vertices) {
			data = vertices.data();
			size = N;
		}

		Vertex* data;
		size_t size;
	};

	class RenderSurface {
		void Clear(Color color) {}
		void Draw(Drawable& drawable) {
			drawable.Draw(context);
		}

		void Draw(const VertexArray& vertices) {}

	private:
		GraphicsContext* context;
	};

	class Renderer {
	public:
		using API = API;

		virtual bool Initialize() = 0;
		virtual void Clear() = 0;
		virtual void SetClearColor(Color color) = 0;
		virtual void Shutdown() = 0;

		virtual void BeginRenderFrame() {}
		virtual void EndRenderFrame() {}

		virtual API GetAPI() = 0;
		virtual const std::string GetAPIString() const = 0;

		virtual GraphicsContext* CreateWindowGraphicsContext(SDL_Window* window) = 0;
		virtual ~Renderer() {}
	};
} // namespace andromeda::graphics

namespace andromeda {
	inline std::string to_string(const graphics::Vertex& vertex) {
		return std::format("position: {}, color: {}", to_string(vertex.position), to_string(vertex.color));
	}

	inline std::string to_string(const graphics::VertexArray& array) {
		std::stringstream ss;

		for (int i = 0; i < array.size; i++) {
			ss << to_string(array.data[i]);
			if (i < array.size - 1) {
				ss << ", ";
			}
		}

		return ss.str();
	}

} // namespace andromeda