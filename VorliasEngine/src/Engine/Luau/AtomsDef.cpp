#include "Engine/Luau/AtomsDef.h"
#include <cstdint>
#include <string>
#include <unordered_map>


const std::unordered_map<std::string_view, int16_t> s_LuauAtoms = {
	{"magnitude", ATOM_MAGNITUDE},
	{"normalized", ATOM_NORMALIZED},
};