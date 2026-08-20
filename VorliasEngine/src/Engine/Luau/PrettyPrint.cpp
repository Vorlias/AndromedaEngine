#include "Engine/Luau/PrettyPrint.h"
#include "lualib.h"
#include "Engine/Log.h"
#include <string.h>

static constexpr const int kMaxDepth = 20;
static constexpr const int kIndentSpaces = 2;

static void array_to_string(lua_State* L, luaL_Strbuf* string_buf, int idx) {
	luaL_addchar(string_buf, '{');
	luaL_addchar(string_buf, '}');
}

static void indent(luaL_Strbuf* string_buf, int level) {
	int spaces = kIndentSpaces * level;
	for (int i = 0; i < spaces; i++) {
		luaL_addchar(string_buf, ' ');
	}
}

static void key_to_string(lua_State* L, luaL_Strbuf* string_buf, int idx, int level);
static void value_to_string(lua_State* L, luaL_Strbuf* string_buf, int idx, int level);

static void table_to_string(lua_State* L, luaL_Strbuf* string_buf, int idx, int level) {
	idx = lua_absindex(L, idx);
	lua_checkstack(L, 4);

	level += 1;
	if (level >= kMaxDepth) {
		luaL_addstring(string_buf, "{ ... }");
		return;
	}

	// Check for __tostring metamethod:
	if (luaL_callmeta(L, idx, "__tostring") != 0) {
		luaL_addvalue(string_buf);
		return;
	}

	luaL_addchar(string_buf, '{');
	bool empty = true;
	bool isArray = lua_objlen(L, idx) > 0;

	std::string tbl_str;

	for (int index = 0; index = lua_rawiter(L, idx, index), index >= 0;) {
		luaL_Strbuf sub_string_buf;

		luaL_buffinit(L, &sub_string_buf);
		if (empty) {
			empty = false;
			luaL_addchar(&sub_string_buf, '\n');
		}

		indent(&sub_string_buf, level);

		// Key:
		if (!isArray || !lua_isnumber(L, -2)) {
			key_to_string(L, &sub_string_buf, -2, level);

			// Separator:
			luaL_addstring(&sub_string_buf, " = ");
		}

		// Value:
		value_to_string(L, &sub_string_buf, -1, level);

		luaL_addstring(&sub_string_buf, ",\n");

		luaL_pushresult(&sub_string_buf);
		size_t l;
		const char* s = lua_tolstring(L, -1, &l);
		tbl_str += std::string(s, l);

		lua_pop(L, 3);
	}

	luaL_addlstring(string_buf, &tbl_str.data()[0], tbl_str.length());

	if (empty) {
		luaL_addstring(string_buf, " <EMPTY> }");
	} else {
		indent(string_buf, level - 1);
		luaL_addstring(string_buf, "}");
	}
}

static void string_to_string(lua_State* L, luaL_Strbuf* string_buf, int idx) {
	size_t len;
	const char* str = lua_tolstring(L, idx, &len);

	luaL_addchar(string_buf, '"');
	for (size_t i = 0; i < len; i++) {
		char c = str[i];
		switch (c) {
			case '\n':
				luaL_addstring(string_buf, "\\n");
				break;
			case '\t':
				luaL_addstring(string_buf, "\\t");
				break;
			case '\r':
				luaL_addstring(string_buf, "\\r");
				break;
			case '\a':
				luaL_addstring(string_buf, "\\a");
				break;
			case '\f':
				luaL_addstring(string_buf, "\\f");
				break;
			case '\v':
				luaL_addstring(string_buf, "\\v");
				break;
			case '"':
				luaL_addstring(string_buf, "\\\"");
				break;
			default:
				luaL_addchar(string_buf, c);
				break;
		}
	}
	luaL_addchar(string_buf, '"');
}

static void key_to_string(lua_State* L, luaL_Strbuf* string_buf, int idx, int level) {
	int type = lua_type(L, idx);

	switch (type) {
		case LUA_TTABLE:
			table_to_string(L, string_buf, idx, level);
			break;
		default:
			luaL_addvalueany(string_buf, idx);
			break;
	}
}

static void value_to_string(lua_State* L, luaL_Strbuf* string_buf, int idx, int level) {
	int type = lua_type(L, idx);

	switch (type) {
		case LUA_TTABLE:
			table_to_string(L, string_buf, idx, level);
			break;
		case LUA_TSTRING:
			string_to_string(L, string_buf, idx);
			break;
		default:
			luaL_addvalueany(string_buf, idx);
			break;
	}
}

void andromeda_luau::luaL_debugstack(lua_State* L) {
	

	int top = lua_gettop(L);

	std::cout << "=== STACK " << L << " sizeof(" << top << ") ===" << std::endl; 
    for (int i = top; i >= 1; i--) {
        int type = lua_type(L, i);
        const char* typeName = luaL_typename(L, i);
        std::cout << i << ": " << typeName << std::endl;
    }
}

void andromeda_luau::luaL_pushprettystring(lua_State* L, int idx) {
	luaL_checktype(L, idx, LUA_TTABLE);

	luaL_Strbuf string_buffer;
	luaL_buffinit(L, &string_buffer);

	value_to_string(L, &string_buffer, idx, 0);
	luaL_pushresult(&string_buffer);
}

const char* andromeda_luau::luaL_toprettylstring(lua_State* L, int idx, size_t* len) {
	luaL_pushprettystring(L, idx);

	if (len == NULL) {
		const char* str = lua_tostring(L, idx);
		lua_pop(L, 1);
		return str;
	} else {
		const char* str = lua_tolstring(L, -1, len);
		lua_pop(L, 1);
		return str;
	}
}