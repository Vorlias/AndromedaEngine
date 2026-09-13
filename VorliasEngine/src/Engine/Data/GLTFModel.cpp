#include "Engine/Data/GLTFModel.h"
#include "Engine/Log.h"
#include "tinygltf/tiny_gltf_v3.h"
#include "stb_image/stb_image.h"

namespace andromeda {
    static std::vector<Image> loadImages(const tg3_model& model, const std::filesystem::path& imageDir) {
        std::vector<Image> images(model.images_count);

        for (int i = 0; i < model.images_count; i++) {
            Image& img = images[i];

            std::filesystem::path imagePath = imageDir / model.images[i].uri.data;
            img.data = stbi_load(imagePath.string().c_str(), &img.size.x, &img.size.y, &img.channels, 4);
            img.type = Image::ImageType_STBI;

            if (!img.data) {
                //error
            }
        }

        return images;
    }

    static std::vector<MaterialData> loadMaterials(const tg3_model& model, const std::vector<Image>& textures) {

    }

	GLTFModel::GLTFModel(const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) return;

		tg3_model model;
		tg3_parse_options opts;
		tg3_error_stack errors;

		tg3_parse_options_init(&opts);
		tg3_error_stack_init(&errors);

		std::string fsPath = filePath.string();

		tg3_error_code parseResult = tg3_parse_file(&model, &errors, fsPath.c_str(), fsPath.length(), &opts);
        if (parseResult != TG3_OK) {
            andromeda::error("Failed to parse GLTF file:");
            for (int i = 0; i < errors.count; i++) {
                andromeda::error("{}", errors.entries[i].message);
            }
            
            tg3_error_stack_free(&errors);
            return;
        }

        tg3_error_stack_free(&errors);
        
        auto imageDir = filePath.parent_path();
        images = loadImages(model, imageDir);
	}
} // namespace andromeda