#if ANDROMEDA_OPENGL
#	include "Engine/Graphics/OpenGL/OpenGLShader.h"
#	include "Engine/File.h"
#	include "Engine/Log.h"
#	include "glad/glad.h"
#	include "Engine/Graphics/OpenGL/OpenGLHelpers.h"

using namespace andromeda::graphics;

OpenGLShader::OpenGLShader() {
	m_programId = glCreateProgram();
}

bool OpenGLShader::LoadFromFile(const std::string& fileName, ShaderType shaderType) {
	int status = GL_FALSE;
	char errorLog[512];

	auto src = ReadFile(fileName);
	if (src.empty()) {
		andromeda::error("Failed to load shader at " + fileName);
		return false;
	}

	uint32_t shaderId;
	if (fileName.ends_with(".spv")) {
		status = CompileShaderBytes(m_programId, &shaderId, shaderType, {src.cbegin(), src.cend()});
		if (status == GL_TRUE) {
			andromeda::trace("Loaded SPIRV shader from file: " + fileName);
		}

	} else {
		status = CompileShaderSource(m_programId, &shaderId, shaderType, src);
	}



	return true;
}

bool OpenGLShader::LoadSPIRV(const std::vector<char>& data, ShaderType shaderType, const char* entryPoint) {
	uint32_t shaderId;
	int status = CompileShaderBytes(m_programId, &shaderId, shaderType, data, entryPoint);
	return status == GL_TRUE;
}

GLenum GetShaderFromType(const ShaderType type) {
	switch (type) {
		case ShaderType::Fragment:
			return GL_FRAGMENT_SHADER;
		case ShaderType::Vertex:
			return GL_VERTEX_SHADER;
	}
}

void replaceAll(std::string& str, const std::string& search, const std::string& replace) {
	for (size_t pos = 0;; pos += replace.length()) {
		// Locate the substring to replace
		pos = str.find(search, pos);
		if (pos == std::string::npos)
			break;
		// Replace by erasing and inserting
		str.erase(pos, search.length());
		str.insert(pos, replace);
	}
}

int OpenGLShader::CompileShaderBytes(
	uint32_t programId,
	uint32_t* id,
	ShaderType shaderType,
	const std::vector<char>& bytes,
	const char* entryPoint
) {
	GLenum glShaderType = GetShaderFromType(shaderType);

	// andromeda::print("GL compile " + std::to_string(GLVersion.major) + "" + std::to_string(GLVersion.minor) + "0");

	int status = GL_FALSE;
	char errorLog[512];
	uint32_t shaderId = glCreateShader(glShaderType);
	{
		glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V, bytes.data(), bytes.size());

		glSpecializeShader(shaderId, entryPoint, 0, 0, 0);

		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
		if (status) {
			glAttachShader(m_programId, shaderId);
			m_shaderIds.emplace_back(shaderId);
			ANDROMEDA_CHECK_GL_ERROR;
		} else {
			return -1;
		}
	}

	*id = shaderId;
	return status;
}

int OpenGLShader::CompileShaderSource(uint32_t programId, uint32_t* id, ShaderType shaderType, const std::string& source) {
	GLenum glShaderType = GetShaderFromType(shaderType);

	std::string compileSource = source;

	// unfortunately need to insert below version lol
	auto firstLineEnd = compileSource.find('\n');
	if (!compileSource.substr(0, firstLineEnd).starts_with("#version")) {
		error("Shader must start with #version");
		return -1;
	}

	std::string shaderHeader = "";
	switch (shaderType) {
		case ShaderType::Fragment:
			shaderHeader = "#define FRAGMENT_SHADER 1";
			break;
		case ShaderType::Vertex:
			shaderHeader = "#define VERTEX_SHADER 1";
			break;
	}

	// This will tell the shader it's OPENGL + ensure it can still do vulkan stuff - ideally we use SPIRV
	compileSource.insert(
		firstLineEnd + 1, "#define ANDROMEDA_OPENGL 1\n#extension GL_KHR_vulkan_glsl : enable\n#define ANDROMEDA_MAIN main\n" + shaderHeader + "\n"
	);

	int status = GL_FALSE;
	char errorLog[512];
	uint32_t shaderId = glCreateShader(glShaderType);
	{
		const GLchar* glSource[] = {compileSource.c_str()};
		glShaderSource(shaderId, 1, glSource, NULL);
		glCompileShader(shaderId);

		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			glGetShaderInfoLog(shaderId, sizeof(errorLog), NULL, errorLog);
			andromeda::error("Failed to compile shader: " + std::string(errorLog));
			glDeleteShader(shaderId);
			return -1;
		} else {
			glAttachShader(programId, shaderId);
			ANDROMEDA_CHECK_GL_ERROR;
		}
	}

	*id = shaderId;
	return status;
}

int OpenGLShader::LinkProgram() {
	int status = GL_FALSE;
	char errorLog[512];

	glLinkProgram(m_programId);
	ANDROMEDA_CHECK_GL_ERROR;
	glValidateProgram(m_programId);
	ANDROMEDA_CHECK_GL_ERROR;
	glGetProgramiv(m_programId, GL_LINK_STATUS, &status);
	ANDROMEDA_CHECK_GL_ERROR;
	if (status != GL_TRUE) {
		glGetProgramInfoLog(m_programId, sizeof(errorLog), NULL, errorLog);
		ANDROMEDA_CHECK_GL_ERROR;
		spdlog::error("Shader linker error: {}", errorLog);
		glDeleteProgram(m_programId);
		m_programId = -1;
	}

	for (auto shaderId : m_shaderIds) {
		glDeleteShader(shaderId);
		ANDROMEDA_CHECK_GL_ERROR;
	}

	m_shaderIds.clear();
	return status;
}

void OpenGLShader::Bind() const {
	glUseProgram(m_programId);
	ANDROMEDA_CHECK_GL_ERROR;
}

void OpenGLShader::Unbind() const {
	glUseProgram(0);
	ANDROMEDA_CHECK_GL_ERROR;
}

void OpenGLShader::Unload() {
	if (glIsProgram(m_programId) == GL_FALSE)
		return;

	int res = GL_FALSE;

	glGetProgramiv(m_programId, GL_DELETE_STATUS, &res);
	ANDROMEDA_CHECK_GL_ERROR;

	if (res == GL_TRUE)
		return;

	glDeleteProgram(m_programId);
	andromeda::trace("Unloaded OpenGL shader");
	ANDROMEDA_CHECK_GL_ERROR;

	m_programId = 0;
}

OpenGLShader::~OpenGLShader() {
	glUseProgram(0);
	ANDROMEDA_CHECK_GL_ERROR;

	Unload();
}

#endif