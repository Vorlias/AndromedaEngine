#include "lua.h"

constexpr const char* kLuauState = "LuauStates";
constexpr const char* kThreads = "LuauThreads";

namespace ENGINE_NS {
	enum class LuauStateContext {
		Game = 1 << 0,

		Compiler = 10000,
	};

	class LuauState {
		lua_State* L;

	public:
		LuauState(LuauStateContext context);
		
		~LuauState();

		lua_State* GetLuaState();
		inline bool IsValid() { return L != nullptr; }

		static LuauState* GetLuauState(lua_State* L);
	};
} // namespace ENGINE_NS::scripting