#pragma once
#include "Engine/Graphics/Shader.h"

#if ANDROMEDA_OPENGL
namespace andromeda::graphics {
	class OpenGLShader : public Shader {
	public:
		OpenGLShader();
		~OpenGLShader() override;

		bool LoadFromFile(const std::string& fileName, ShaderType shaderType);
		bool LoadSPIRV(const std::vector<char>& data, ShaderType shaderType, const char* entryPoint = "main") override;

		int LinkProgram();
		void Bind() const;
		void Unbind() const;

		void Unload() override;
	private:
		int CompileShaderBytes(
			uint32_t programId,
			uint32_t* shaderId,
			ShaderType shaderType,
			const std::vector<char>& bytes,
			const char* entryPoint = "main"
		);
		int CompileShaderSource(uint32_t programId, uint32_t* shaderId, ShaderType shaderType, const std::string& source);

		uint32_t m_programId{0};
        std::vector<uint32_t> m_shaderIds{};
	};
}; // namespace andromeda::graphics
#endif