#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

extern const std::unordered_map<std::string_view, int16_t> s_LuauAtoms;

inline constexpr int16_t ATOM_MAGNITUDE = 1; // atom for magnitude
inline constexpr int16_t ATOM_NORMALIZED = 2; // atom for normalized
inline constexpr int16_t ATOM_DISTANCE = 3; // atom for Distance
inline constexpr int16_t ATOM_SHRINK = 4; // atom for Shrink
inline constexpr int16_t ATOM_EXTEND = 5; // atom for Extend