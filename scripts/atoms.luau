local fs = require("@std/fs")

local atoms = {
    "position",
    "size",
    "scale",
    "rotation",

    -- Entity
    "GetComponent",
    "AddComponent",
    "HasComponent",

    -- Vector3
    "magnitude",
    "normalized",
    "Distance",
    "Shrink",

    -- Vector2
    "Extend",

    -- Rect
    "min",
    "max",
    
    "Contains",

    "x",
    "y",
    "z",

    "width",
    "height",
    "center",

    "GetNormalizedVector",
    "GetAbsoluteVector",

    -- Bounds
    "Encapsulate",
    "Intersects",
    "extents",
    "Expand",
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

local function uppercaseNameify(value: string): string
    local newValue = ""
    for i = 1, #value do
        local c = value:sub(i, i)
        if c:upper() == c and i ~= 1 then
            newValue ..= "_"
        end
        
        newValue ..= c:upper()
    end
    return newValue
end

local atomSet: {[string]: boolean} = {}
for i, atom in atoms do
    if not atomSet[atom:lower()] then
        atomSet[atom:lower()] = true
        table.insert(headerFile, `inline constexpr int16_t ATOM_{uppercaseNameify(atom)} = {i}; // atom for {atom}`)
    end
    table.insert(sourceFile, `\t\{"{atom}", ATOM_{uppercaseNameify(atom)}},`)
end
table.insert(sourceFile, "};")

fs.writeStringToFile(`VorliasEngine/include/{headerFilePath}`, table.concat(headerFile, "\n"))
fs.writeStringToFile(`VorliasEngine/src/Engine/Luau/{sourceFilePath}`, table.concat(sourceFile, "\n"))
print("Generated atoms for the Luau Runtime")