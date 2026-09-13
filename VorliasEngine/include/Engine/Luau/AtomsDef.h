#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

extern const std::unordered_map<std::string_view, int16_t> s_LuauAtoms;

inline constexpr int16_t ATOM_POSITION = 1; // atom for position
inline constexpr int16_t ATOM_SIZE = 2; // atom for size
inline constexpr int16_t ATOM_SCALE = 3; // atom for scale
inline constexpr int16_t ATOM_ROTATION = 4; // atom for rotation
inline constexpr int16_t ATOM_GET_COMPONENT = 5; // atom for GetComponent
inline constexpr int16_t ATOM_ADD_COMPONENT = 6; // atom for AddComponent
inline constexpr int16_t ATOM_HAS_COMPONENT = 7; // atom for HasComponent
inline constexpr int16_t ATOM_MAGNITUDE = 8; // atom for magnitude
inline constexpr int16_t ATOM_NORMALIZED = 9; // atom for normalized
inline constexpr int16_t ATOM_DISTANCE = 10; // atom for Distance
inline constexpr int16_t ATOM_SHRINK = 11; // atom for Shrink
inline constexpr int16_t ATOM_EXTEND = 12; // atom for Extend
inline constexpr int16_t ATOM_MIN = 13; // atom for min
inline constexpr int16_t ATOM_MAX = 14; // atom for max
inline constexpr int16_t ATOM_CONTAINS = 15; // atom for Contains
inline constexpr int16_t ATOM_X = 16; // atom for x
inline constexpr int16_t ATOM_Y = 17; // atom for y
inline constexpr int16_t ATOM_Z = 18; // atom for z
inline constexpr int16_t ATOM_WIDTH = 19; // atom for width
inline constexpr int16_t ATOM_HEIGHT = 20; // atom for height
inline constexpr int16_t ATOM_CENTER = 21; // atom for center
inline constexpr int16_t ATOM_GET_NORMALIZED_VECTOR = 22; // atom for GetNormalizedVector
inline constexpr int16_t ATOM_GET_ABSOLUTE_VECTOR = 23; // atom for GetAbsoluteVector
inline constexpr int16_t ATOM_ENCAPSULATE = 24; // atom for Encapsulate
inline constexpr int16_t ATOM_INTERSECTS = 25; // atom for Intersects
inline constexpr int16_t ATOM_EXTENTS = 26; // atom for extents
inline constexpr int16_t ATOM_EXPAND = 27; // atom for Expand