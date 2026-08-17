#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

extern const std::unordered_map<std::string_view, int16_t> s_LuauAtoms;

inline constexpr int16_t ATOM_MAGNITUDE = 1; // atom for magnitude
inline constexpr int16_t ATOM_NORMALIZED = 2; // atom for normalized