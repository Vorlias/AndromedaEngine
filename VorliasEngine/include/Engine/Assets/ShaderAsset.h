#pragma once
#include "../Asset.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Utility/SourceParser.h"
#include <sstream>

namespace andromeda {
	class FragmentShaderFile : public Asset {
	public:
		FragmentShaderFile(const std::string& path) : Asset(AssetType::ShaderFile, UUID{}, path) {}
	};

    class VertexShaderFile : public Asset {
	public:
		VertexShaderFile(const std::string& path) : Asset(AssetType::ShaderFile, UUID{}, path) {}
    };

	class ShaderModuleParser{
	public:
		ShaderModuleParser(const std::string source);
		void Parse();
	private:
		std::string m_vertex;
		std::string m_fragment;

		SourceParser m_parser;
	};

	class ShaderModuleFile : public Asset {
	public:
		ShaderModuleFile(const std::string& path) : Asset(AssetType::ShaderFile, UUID{}, path) {}
	};
} // namespace andromeda