#pragma once
#include <sstream>
#include <assert.h>

namespace andromeda {
	inline bool IsNumeric(char v) {
		return std::isdigit(static_cast<unsigned char>(v));
	}

	inline bool IsAlpha(char v) {
		return std::isalpha(static_cast<unsigned char>(v));
	}

	inline bool IsAlphaNumeric(char v) {
		return std::isalnum(static_cast<unsigned char>(v));
	}

	inline bool IsPunctuation(char v) {
		return std::ispunct(static_cast<unsigned char>(v));
	}

	inline bool IsWhitespace(char v) {
		return v == ' ' || v == '\t';
	}

	inline bool IsNewLine(char v) {
		return v == '\n' || v == '\r';
	}

	// A more source-parse friendly wrapper around sstream
	class SourceParser {
	public:
		SourceParser(std::string source) : m_source(source) {}

		char PeekNext() {
			if (p + 1 >= m_source.length())
				return 0;
			return m_source.at(p + 1);
		}

		char Next() {
			// assert(p + 1 < m_source.length());
			if (p + 1 >= m_source.length()) return 0;

			return m_source.at(++p);
		}

		char Current() {
			if (p >= m_source.length())
				return 0;
			return m_source.at(p);
		}

		char PeekPrev() {
			return m_source.at(p - 1);
		}

		bool HasNext() {
			return p + 1 < m_source.length();
		}

		std::string Lookahead(size_t length) {
			return m_source.substr(p + 1, length);
		}

		void MoveNext(size_t count) {
			p += count;
		}

		void Reset() {
			p = -1;
		}

		std::string ReadUntil(char token) {
			std::string value;

			for (; m_source.at(p) != token; p++) {
				if (p == token)
					return value;
				value += m_source.at(p);
			}

			return value;
		}

		std::string ReadWhile(bool (*predicate)(char)) {
			std::string value;
			while (p < m_source.length() && predicate(m_source.at(p))) {
				value += m_source.at(p);
				p++;
			}
			return value;
		}


		void SkipWhitespace() {
			while (IsWhitespace(m_source.at(p)) || IsNewLine(m_source.at(p))) {
				p++;
			}
		}

		std::string ReadLine() {
			std::string line;

			SkipWhitespace();
			while (p < m_source.size() && !IsNewLine(m_source.at(p))) {
				line += m_source.at(p);
				p++;
			}
			p++; // skip newline

			return line;
		}

	private:
		int p = 0;
		std::string m_source;
	};
} // namespace andromeda