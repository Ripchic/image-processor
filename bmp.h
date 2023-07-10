#pragma once
#include <vector>
#include <cstdint>

namespace constants {
const int MAX_VAL = 255;
const int MIN_VAL = 0;
const int MED_VAL = 128;
const float COEF_OFIVE = 0.5;
const std::vector<std::vector<float>> SHARP_MATRIX = {{{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}};
const std::vector<std::vector<float>> EDGE_MATRIX = {{{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}}};
}  // namespace constants

class BGR {
public:
    uint8_t b = constants::MIN_VAL, g = constants::MIN_VAL, r = constants::MIN_VAL;
    BGR() {
    }
    BGR(uint8_t b, uint8_t g, uint8_t r) : b(b), g(g), r(r) {
    }
};

struct BITMAPFILEHEADER {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} __attribute__((packed));

struct BITMAPINFOHEADER {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} __attribute__((packed));

class Image {
public:
    explicit Image(const char* filename);
    void Save(const char* filename);
    void Load(const char* filename);
    size_t Padding() const;
    BITMAPFILEHEADER file_header_;
    BITMAPINFOHEADER info_header_;
    std::vector<std::vector<BGR>> img_;
};