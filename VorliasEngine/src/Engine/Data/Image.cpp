#include "Engine/Data/Image.h"
using namespace andromeda;

static uint32_t s_whitePixelData = 0xFF'FF'FF'FF;

Image Image::WHITE_PIXEL(Image::RGBA, Vector2i(1, 1), reinterpret_cast<unsigned char*>(&s_whitePixelData));