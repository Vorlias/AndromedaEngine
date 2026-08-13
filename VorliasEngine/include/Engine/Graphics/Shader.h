#pragma once
#include "Engine/Common.h"
#include "Engine/Memory.h"
#include <unordered_map>

namespace andromeda::graphics {
	enum class ShaderType {
		Vertex,
		Fragment,
	};

	class Shader : public RefCounted {
		static SharedRef<Shader> LoadShaderFromFile(const std::string& fileName, ShaderType type);

	public:
		virtual bool LoadSPIRV(const std::vector<char>& data, ShaderType shaderType, const char* entryPoint = "main") = 0;
		virtual void Unload() = 0;
	private:
	};

	class ShaderLibrary {
    public:
        // Ref<Shader> LoadShader(const std::string& name, const std::string& fileName);
		Ref<Shader> LoadShaderFromFiles(const std::string& name, const std::string& vertexFileName, const std::string& fragmentFileName);

		void UnloadAllShaders();
	private:
		std::unordered_map<std::string, Ref<Shader>> m_shaders;
	};
} // namespace andromeda::graphics