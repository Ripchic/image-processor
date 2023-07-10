#include "bmp.h"
#include <fstream>
#include <iostream>
#include <memory>

Image::Image(const char* filename) {
    Load(filename);
}

size_t Image::Padding() const {
    size_t width = info_header_.biWidth * 3;
    size_t padding = (4 - width % 4) % 4;
    return width + padding;
}

void Image::Load(const char* filename) {
    try {
        std::ifstream fin(filename, std::ios::binary);
        if (!fin) {
            throw std::runtime_error("Failed to open file for saving\n");
        }
        fin.read(reinterpret_cast<char*>(&file_header_), sizeof(file_header_));
        fin.read(reinterpret_cast<char*>(&info_header_), sizeof(info_header_));
        img_.resize(info_header_.biHeight, std::vector<BGR>(info_header_.biWidth));
        size_t pad = Padding();
        std::unique_ptr<unsigned char[]> rgb(new unsigned char[file_header_.bfSize - file_header_.bfOffBits]);
        fin.read(reinterpret_cast<char*>(rgb.get()), file_header_.bfSize - file_header_.bfOffBits);
        for (size_t i = 0; i < info_header_.biHeight; ++i) {
            for (size_t j = 0; j < info_header_.biWidth; ++j) {
                img_[i][j].b = rgb[i * pad + 3 * j + 0];
                img_[i][j].g = rgb[i * pad + 3 * j + 1];
                img_[i][j].r = rgb[i * pad + 3 * j + 2];
            }
        }
    } catch (std::runtime_error& e) {
        std::cerr << e.what();
    } catch (...) {
        std::cerr << "Image load error\n";
    }
}

void Image::Save(const char* filename) {
    try {
        std::ofstream fout(filename, std::ios::binary);
        if (!fout) {
            throw std::runtime_error("Failed to open file for writing");
        }
        fout.write(reinterpret_cast<const char*>(&file_header_), sizeof(file_header_));
        size_t pad = Padding();
        fout.write(reinterpret_cast<const char*>(&info_header_), sizeof(info_header_));
        std::unique_ptr<unsigned char[]> rgb(new unsigned char[info_header_.biHeight * pad * 3]);
        for (size_t i = 0; i < info_header_.biHeight; ++i) {
            for (size_t j = 0; j < info_header_.biWidth; ++j) {
                rgb[i * pad + 3 * j + 0] = img_[i][j].b;
                rgb[i * pad + 3 * j + 1] = img_[i][j].g;
                rgb[i * pad + 3 * j + 2] = img_[i][j].r;
            }
        }
        fout.write(reinterpret_cast<const char*>(rgb.get()), file_header_.bfSize - file_header_.bfOffBits);
        fout.write(reinterpret_cast<const char*>(&file_header_), sizeof(file_header_));
    } catch (std::runtime_error& e) {
        std::cerr << e.what();
    } catch (...) {
        std::cerr << "Image save error\n";
    }
}