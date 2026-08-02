#include <string>
#include "lua.h"


namespace ENGINE_NS {
	struct LuauCompileResult {
		char* compiled_data;
		int compiled_data_size;
		bool compiled;
	};

	class LuauScript {
        bool CompileSource(char* source, int source_len, char* file_name, int file_name_len, int optimization_level);
	public:
		static LuauScript LoadFromMemory(std::string source, std::string name);
	
		inline bool HasError() {
			return m_errlen > 0;
		}

		inline std::string GetError() const {
			return std::string(m_err, m_errlen);
		}

		LuauScript();
		~LuauScript();

        bool LoadThread();
        bool RunThread();
    private:
        lua_State* m_thread;

        const char* m_fileName;

		char m_err[128];
		size_t m_errlen = 0;

		char m_bytecode[1024 * 512];
		size_t m_bytecodeSize = 0;
	};
} // namespace ENGINE_NS::scripting