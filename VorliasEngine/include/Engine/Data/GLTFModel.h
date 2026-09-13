#pragma once 
#include <filesystem>

#include "Image.h"
#include "Material.h"


namespace andromeda {
    struct GLTFModel {
        GLTFModel(const std::filesystem::path& filePath);

        std::vector<Image> images{};
        std::vector<MaterialData> materials{};
    };
}