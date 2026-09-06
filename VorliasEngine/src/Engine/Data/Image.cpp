#include "Engine/Data/Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG //generate user friendly error messages
#include "stb_image/stb_image.h"
#include "SDL3/SDL.h"

using namespace andromeda;

static uint32_t s_whitePixelData = 0x00'00'00'00;

Image Image::WHITE_PIXEL(Image::RGBA, Vector2i(1, 1), reinterpret_cast<unsigned char*>(&s_whitePixelData));

Image::Image(const std::filesystem::path& filePath, Channels chanTarget): type(ImageType_STBI), data(nullptr) {
    auto absPath = std::filesystem::absolute(filePath).string();
    data = stbi_load(absPath.c_str(), &size.x, &size.y, &channels, static_cast<int>(chanTarget)); 
}

void Image::Destroy() {
    if (type == ImageType_STBI) stbi_image_free(data);
}