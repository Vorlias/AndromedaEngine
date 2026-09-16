#include "Engine/Data.h"
#include "Engine/Data/Vertex.h"

static std::vector<andromeda::Vertex> s_vertices;
static std::vector<uint32_t> s_indices;

bool andromeda::LoadData() {
	constexpr size_t vertexBufferBytes = 64 * 1024 * 1024; // 64MB verts budget
	constexpr size_t indexBufferBytes = 32 * 1024 * 1024; // 32MB index budget
	constexpr size_t totalVerts = vertexBufferBytes / sizeof(Vertex);
	constexpr size_t totalIndices = indexBufferBytes / sizeof(uint32_t);
    
    s_vertices.reserve(totalVerts);
    s_indices.resize(totalIndices);
}