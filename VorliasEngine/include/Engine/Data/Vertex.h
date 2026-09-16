#pragma once
#include "Vector.h"
#include "Color.h"
#include <format>

namespace andromeda {
	struct Vertex {
		static constexpr uint32_t elements = Vector3::elements * 2 + Vector2::elements + Color3::elements;
		static constexpr size_t bytes = elements * sizeof(float);
		using Array = std::array<float, elements>;

		Vector3 position{};
		Color3 color{};
		Vector3 normal{};
		Vector2 uv{};

		Array ToArray() const {
			return {
				position.x,
				position.y,
				position.z,
				color.r,
				color.g,
				color.b,
				normal.x,
				normal.y,
				normal.z,
				uv.x,
				uv.y,
			};
		}
	};

	struct VertexArray {
		VertexArray() : data(nullptr), size(0) {}

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

	inline std::string to_string(const Vertex& vertex) {
		if (vertex.normal != Vector3() || vertex.uv != Vector2()) {
			return std::format(
				"<{}, {}, {}, {}>", to_string(vertex.position), to_string((Vector3)vertex.color), to_string(vertex.normal), to_string(vertex.uv)
			);
		} else {
			return std::format("<{}, {}, ...>", to_string(vertex.position), to_string((Vector3) vertex.color));
		}
	}

	inline std::string to_string(const VertexArray& array) {
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