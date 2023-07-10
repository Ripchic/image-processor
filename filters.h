#pragma once
#include "bmp.h"
#include <cmath>
#include <vector>

class Matrix {
public:
    BGR MatrixApply(Image& image, std::vector<std::vector<float>> matrix, size_t row, size_t column);
};

class Negative {
public:
    void NegativeApply(Image& image);
};

class GrayScale {
public:
    void GrayScaleApply(Image& image);
};

class Crop {
public:
    void CropApply(Image& image, int x, int y);
};

class Sharpening : public Matrix {
public:
    void SharpeningApply(Image& image);
};

class EdgeDetection : public Matrix {
public:
    void EdgeDetectionApply(Image& image, float threshold);
};

class Gaussianblur : public Matrix {
public:
    void GaussianBlurApply(Image& image, float sigma);
    std::vector<std::vector<float>> GenerateKernel(float sigma);
};
class Pixelate {
public:
    void PixelateApply(Image& image, int cell_size);
};
class Emboss : public Matrix {
public:
    void EmbossApply(Image& image);
};

class Noise {
public:
    void NoiseApply(Image& image, int intensity);
};

class Warholization {
public:
    void WarholizationApply(Image& image);
};

class Twirl {
public:
    void TwirlApply(Image& image, int radius, double angle);
};