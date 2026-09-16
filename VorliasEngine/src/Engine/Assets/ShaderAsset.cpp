#include "Engine/Assets/ShaderAsset.h"
using namespace andromeda;


ShaderModuleParser::ShaderModuleParser(const std::string source) : m_parser(source) {}

std::string pragma(std::stringstream& stream) {
	std::string pragmaText{};
	while (stream.peek() != 0 && stream.peek() != ' ') {
		stream >> pragmaText;
	}

	return pragmaText;
}

std::string parseStringLiteral(std::stringstream& stream) {
	char token;

	stream >> token;
	assert(token == '"');

	stream >> token;
	std::string literal{};
	literal += token;

	while (token != '"') {
		stream >> token;
		literal += token;
	}

	return literal;
}

enum ModuleMode {
	Metadata,
	FragmentShader,
	VertexShader,
};

static std::vector<std::string> split(std::string source, std::string delim) {
	std::vector<std::string> values;

	size_t startPos = 0;
	size_t pos;
	while ((pos = source.find(delim, startPos)) != std::string::npos) {
		auto substr = source.substr(startPos, pos);

		values.push_back(substr);
		startPos = pos + delim.length(); // skip delim
	}

	auto substr = source.substr(startPos, pos);
	values.push_back(substr);

	return values;
}

static std::string parseStringLiteral(andromeda::SourceParser& parser, char quote) {
	std::string literal = "";

	auto token = parser.Current();
	assert(token == quote);
    parser.Next(); // consume quote

	while (parser.HasNext() && parser.Current() != quote) {
		literal += parser.Current();
        parser.Next();
	}

	assert(parser.Current() == quote);
    parser.Next(); // consume quote

	return literal;
}

class InvalidModuleException : public std::exception {
private:
	std::string message;

public:
	// Constructor accepting const char*
	InvalidModuleException(const char* msg) : message(msg) {}

	// Override what() method, marked
	// noexcept for modern C++
	const char* what() const noexcept {
		return message.c_str();
	}
};

std::string parseModule(andromeda::SourceParser& parser) {
    std::string source;

	while (parser.HasNext()) {
        char c = parser.Next();
        if (c == '#' && parser.Lookahead(3) == "end") {
            parser.MoveNext(4);
            break;
        }

        source += c;
    }

    return source;
}

void ShaderModuleParser::Parse() {
	static ModuleMode moduleMode = Metadata;

	std::stringstream sstream;
	std::string moduleName;
	int version = 0;

    std::string fragmentShader;
    std::string vertexShader;

	m_parser.SkipWhitespace();
	while (m_parser.HasNext()) {
		char c = m_parser.Current(); // return current, move next
		if (c == '#') { // directive
            m_parser.Next();  // skip '#'

			std::string label = m_parser.ReadWhile(IsAlpha);
			if (label == "name" && m_parser.Current() == ' ') {
				m_parser.Next(); // consume ' '
				std::string value = parseStringLiteral(m_parser, '"');

				moduleName = value;
				continue;
			} else if (label == "version") {
				m_parser.Next(); // consume ' '

				sstream << m_parser.ReadWhile(IsNumeric);
				sstream >> version;
				sstream.clear();
				continue;
			} else if (label == "shader") {
				m_parser.Next(); // consume ' '
				std::string type = m_parser.ReadLine();
				if (type == "vertex") {
                    m_vertex = parseModule(m_parser);
				} else if (type == "fragment") {
                    m_fragment = parseModule(m_parser);
				} else {
					throw InvalidModuleException((type + " is not a valid shader module").c_str());
				}
			}
		} else if (c == '/' && m_parser.Current() == '/') {
			m_parser.ReadLine();
			continue;
		} else if (IsWhitespace(c) || IsNewLine(c)) {
            m_parser.SkipWhitespace();
			continue;
		} else {
			assert(false);
		}

        m_parser.Next();
    }

    if (!m_vertex.starts_with("#version")) {
        m_vertex = "#version " + std::to_string(version) + "\n" + m_vertex;
    }

    if (!m_fragment.starts_with("#version")) {
        m_fragment = "#version " + std::to_string(version) + "\n" + m_fragment;
    }
}