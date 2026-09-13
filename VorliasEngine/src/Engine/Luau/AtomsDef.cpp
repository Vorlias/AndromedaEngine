#include "Engine/Luau/AtomsDef.h"
#include <cstdint>
#include <string>
#include <unordered_map>


const std::unordered_map<std::string_view, int16_t> s_LuauAtoms = {
	{"position", ATOM_POSITION},
	{"size", ATOM_SIZE},
	{"scale", ATOM_SCALE},
	{"rotation", ATOM_ROTATION},
	{"GetComponent", ATOM_GET_COMPONENT},
	{"AddComponent", ATOM_ADD_COMPONENT},
	{"HasComponent", ATOM_HAS_COMPONENT},
	{"magnitude", ATOM_MAGNITUDE},
	{"normalized", ATOM_NORMALIZED},
	{"Distance", ATOM_DISTANCE},
	{"Shrink", ATOM_SHRINK},
	{"Extend", ATOM_EXTEND},
	{"min", ATOM_MIN},
	{"max", ATOM_MAX},
	{"Contains", ATOM_CONTAINS},
	{"x", ATOM_X},
	{"y", ATOM_Y},
	{"z", ATOM_Z},
	{"width", ATOM_WIDTH},
	{"height", ATOM_HEIGHT},
	{"center", ATOM_CENTER},
	{"GetNormalizedVector", ATOM_GET_NORMALIZED_VECTOR},
	{"GetAbsoluteVector", ATOM_GET_ABSOLUTE_VECTOR},
	{"Encapsulate", ATOM_ENCAPSULATE},
	{"Intersects", ATOM_INTERSECTS},
	{"extents", ATOM_EXTENTS},
	{"Expand", ATOM_EXPAND},
};