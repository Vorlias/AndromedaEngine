local fs = require("@std/fs")

local atoms = {
    "magnitude",
    "normalized"
}


-- generation code here:
local headerFile = {
    "#pragma once",
    "#include <cstdint>",
    "#include <string>",
    "#include <unordered_map>",
    "",
    "extern const std::unordered_map<std::string_view, int16_t> s_LuauAtoms;",
    "",
}
local headerFilePath = "Engine/Luau/AtomsDef.h"

local sourceFile = {
    `#include "{headerFilePath}"`,
    "#include <cstdint>",
    "#include <string>",
    "#include <unordered_map>",
    "",
    "",
    "const std::unordered_map<std::string_view, int16_t> s_LuauAtoms = {",
}
local sourceFilePath = "AtomsDef.cpp"


for i, atom in atoms do
    table.insert(headerFile, `inline constexpr int16_t ATOM_{atom:upper()} = {i}; // atom for {atom}`)
    table.insert(sourceFile, `\t\{"{atom}", ATOM_{atom:upper()}},`)
end
table.insert(sourceFile, "};")

fs.writeStringToFile(`VorliasEngine/include/{headerFilePath}`, table.concat(headerFile, "\n"))
fs.writeStringToFile(`VorliasEngine/src/Engine/Luau/{sourceFilePath}`, table.concat(sourceFile, "\n"))