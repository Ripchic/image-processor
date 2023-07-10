#include "filters.h"
#include <cmath>
BGR Matrix::MatrixApply(Image& image, std::vector<std::vector<float>> matrix, size_t row, size_t column) {
    BGR pixel = BGR();
    float red = constants::MIN_VAL;
    float green = constants::MIN_VAL;
    float blue = constants::MIN_VAL;
    const int half_matr = static_cast<int>(matrix.size() / 2);
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix[0].size(); ++j) {
            size_t cur_row = row + i - half_matr;
            size_t cur_col = column + j - half_matr;
            cur_row = std::clamp(static_cast<int>(cur_row), 0, static_cast<int>(image.info_header_.biHeight - 1));
            cur_col = std::clamp(static_cast<int>(cur_col), 0, static_cast<int>(image.info_header_.biWidth - 1));
            red += matrix[i][j] * static_cast<float>(image.img_[cur_row][cur_col].r);
            green += matrix[i][j] * static_cast<float>(image.img_[cur_row][cur_col].g);
            blue += matrix[i][j] * static_cast<float>(image.img_[cur_row][cur_col].b);
        }
    }
    pixel.r = std::clamp(static_cast<int>(std::round(red)), constants::MIN_VAL, constants::MAX_VAL);
    pixel.g = std::clamp(static_cast<int>(std::round(green)), constants::MIN_VAL, constants::MAX_VAL);
    pixel.b = std::clamp(static_cast<int>(std::round(blue)), constants::MIN_VAL, constants::MAX_VAL);
    return pixel;
}

void Negative::NegativeApply(Image& image) {
    for (size_t i = 0; i < image.info_header_.biHeight; ++i) {
        for (size_t j = 0; j < image.info_header_.biWidth; ++j) {
            image.img_[i][j].r = constants::MAX_VAL - image.img_[i][j].r;
            image.img_[i][j].g = constants::MAX_VAL - image.img_[i][j].g;
            image.img_[i][j].b = constants::MAX_VAL - image.img_[i][j].b;
        }
    }
}

void GrayScale::GrayScaleApply(Image& image) {
    const float red_coef = 0.299;
    const float green_coef = 0.587;
    const float blue_coef = 0.114;

    for (size_t i = 0; i < image.info_header_.biHeight; ++i) {
        for (size_t j = 0; j < image.info_header_.biWidth; ++j) {
            float color = red_coef * static_cast<float>(image.img_[i][j].r) +
                          green_coef * static_cast<float>(image.img_[i][j].g) +
                          blue_coef * static_cast<float>(image.img_[i][j].b);
            image.img_[i][j].b = static_cast<int>(round(color));
            image.img_[i][j].g = static_cast<int>(round(color));
            image.img_[i][j].r = static_cast<int>(round(color));
        }
    }
}

void Crop::CropApply(Image& image, int x, int y) {
    if (x < image.info_header_.biWidth || y < image.info_header_.biHeight) {
        if (x >= image.info_header_.biWidth) {
            x = image.info_header_.biWidth;
        }
        if (y >= image.info_header_.biHeight) {
            y = image.info_header_.biHeight;
        }
        const int head_size = 54;
        std::vector<std::vector<BGR>> tmp(y, std::vector<BGR>(x));
        for (size_t i = 0; i < y; ++i) {
            for (size_t j = 0; j < x; ++j) {
                tmp[y - i - 1][j] = image.img_[image.info_header_.biHeight - i - 1][j];
            }
        }
        image.img_ = tmp;
        image.info_header_.biWidth = x;
        image.info_header_.biHeight = y;
        image.info_header_.biSizeImage = image.Padding() * image.info_header_.biHeight;
        image.file_header_.bfSize = image.info_header_.biSizeImage + head_size;
    }
}

void Sharpening::SharpeningApply(Image& image) {
    Image& image_pixels = image;
    Image new_pixels = image_pixels;
    BGR current_pixel;
    for (size_t i = 0; i < image_pixels.info_header_.biHeight; ++i) {
        for (size_t j = 0; j < image_pixels.info_header_.biWidth; ++j) {
            current_pixel = MatrixApply(image_pixels, constants::SHARP_MATRIX, i, j);
            new_pixels.img_[i][j] = current_pixel;
        }
    }
    image_pixels = new_pixels;
}

void EdgeDetection::EdgeDetectionApply(Image& image, float threshold) {
    GrayScale _;
    _.GrayScaleApply(image);
    BGR white = {constants::MAX_VAL, constants::MAX_VAL, constants::MAX_VAL};
    BGR black = {constants::MIN_VAL, constants::MIN_VAL, constants::MIN_VAL};
    Image& image_pixels = image;
    Image new_pixels = image_pixels;
    BGR current_pixel;
    threshold *= constants::MAX_VAL;
    for (size_t i = 0; i < image_pixels.info_header_.biHeight; ++i) {
        for (size_t j = 0; j < image_pixels.info_header_.biWidth; ++j) {
            current_pixel = MatrixApply(image_pixels, constants::EDGE_MATRIX, i, j);
            if (static_cast<float>(current_pixel.r) > threshold || static_cast<float>(current_pixel.g) > threshold ||
                static_cast<float>(current_pixel.b) > threshold) {
                new_pixels.img_[i][j] = white;
            } else {
                new_pixels.img_[i][j] = black;
            }
        }
    }
    image_pixels = new_pixels;
}

std::vector<std::vector<float>> Gaussianblur::GenerateKernel(float sigma) {
    std::vector<std::vector<float>> kernel;
    size_t matrix_radius = std::ceil(sigma * 3);
    size_t matrix_size = matrix_radius * 2 + 1;
    std::vector<float> matrix_row(matrix_size);
    float sum = 0;
    for (size_t i = 0; i < matrix_radius + 1 / 2; ++i) {
        float weight = static_cast<float>(std::exp(-constants::COEF_OFIVE * pow(static_cast<float>(i) / sigma, 2)) /
                                          std::sqrt(2 * M_PI * sigma * sigma));
        matrix_row[matrix_radius + i] = weight;
        matrix_row[matrix_radius - i] = weight;
        sum += weight * static_cast<float>(i == 0 ? 1 : 2);
    }
    for (float& val : matrix_row) {
        val /= sum;
    }
    kernel.push_back(matrix_row);
    return kernel;
}

void Gaussianblur::GaussianBlurApply(Image& image, float sigma) {
    std::vector<std::vector<float>> gauss_ker = GenerateKernel(sigma);
    Image& image_pixels = image;
    Image new_pixels = image_pixels;
    BGR current_pixel;
    for (size_t i = 0; i < image_pixels.info_header_.biHeight; ++i) {
        for (size_t j = 0; j < image_pixels.info_header_.biWidth; ++j) {
            current_pixel = MatrixApply(image_pixels, gauss_ker, i, j);
            new_pixels.img_[i][j] = current_pixel;
        }
    }
    image_pixels = new_pixels;

    std::vector<std::vector<float>> transposed_gauss_ker;
    std::vector<float> temp_row;
    for (size_t i = 0; i < gauss_ker[0].size(); ++i) {
        for (size_t j = 0; j < gauss_ker.size(); ++j) {
            float current_value = gauss_ker[j][i];
            temp_row.push_back(current_value);
        }
        transposed_gauss_ker.push_back(temp_row);
        temp_row.clear();
    }

    for (size_t i = 0; i < image_pixels.info_header_.biHeight; ++i) {
        for (size_t j = 0; j < image_pixels.info_header_.biWidth; ++j) {
            current_pixel = MatrixApply(image_pixels, transposed_gauss_ker, i, j);
            new_pixels.img_[i][j] = current_pixel;
        }
    }
    image_pixels = new_pixels;
}

void Pixelate::PixelateApply(Image& image, int cell_size) {
    Image& image_pixels = image;

    for (size_t i = 0; i < image.info_header_.biHeight; i += cell_size) {
        for (size_t j = 0; j < image.info_header_.biWidth; j += cell_size) {
            size_t r_w = std::min(cell_size, static_cast<int>(image.info_header_.biWidth - j));
            size_t r_h = std::min(cell_size, static_cast<int>(image.info_header_.biHeight - i));

            int r_sum = 0;
            int g_sum = 0;
            int b_sum = 0;
            size_t pxl_cnt = r_w * r_h;
            for (size_t y = i; y < i + r_h; y++) {
                for (size_t x = j; x < j + r_w; x++) {
                    r_sum += image.img_[y][x].r;
                    g_sum += image.img_[y][x].g;
                    b_sum += image.img_[y][x].b;
                }
            }
            for (size_t y = i; y < i + r_h; y++) {
                for (size_t x = j; x < j + r_w; x++) {
                    image_pixels.img_[y][x].r = r_sum / pxl_cnt;
                    image_pixels.img_[y][x].g = g_sum / pxl_cnt;
                    image_pixels.img_[y][x].b = b_sum / pxl_cnt;
                }
            }
        }
    }

    image = image_pixels;
}

void Emboss::EmbossApply(Image& image) {
    Image& image_pixels = image;
    Image new_pixels = image_pixels;
    for (size_t i = 1; i < image_pixels.info_header_.biHeight - 1; ++i) {
        for (size_t j = 1; j < image_pixels.info_header_.biWidth - 1; ++j) {
            BGR top_left = image_pixels.img_[i - 1][j - 1];
            BGR top = image_pixels.img_[i - 1][j];
            BGR top_right = image_pixels.img_[i - 1][j + 1];
            BGR left = image_pixels.img_[i][j - 1];
            BGR right = image_pixels.img_[i][j + 1];
            BGR bottom_left = image_pixels.img_[i + 1][j - 1];
            BGR bottom = image_pixels.img_[i + 1][j];
            BGR bottom_right = image_pixels.img_[i + 1][j + 1];
            int emboss_r =
                -top_left.r - top.r - top_right.r + bottom_left.r + bottom.r + bottom_right.r - left.r + right.r;
            int emboss_g =
                -top_left.g - top.g - top_right.g + bottom_left.g + bottom.g + bottom_right.g - left.g + right.g;
            int emboss_b =
                -top_left.b - top.b - top_right.b + bottom_left.b + bottom.b + bottom_right.b - left.b + right.b;
            emboss_r = std::clamp(emboss_r + constants::MED_VAL, 0, constants::MAX_VAL);
            emboss_g = std::clamp(emboss_g + constants::MED_VAL, 0, constants::MAX_VAL);
            emboss_b = std::clamp(emboss_b + constants::MED_VAL, 0, constants::MAX_VAL);
            new_pixels.img_[i][j].r = emboss_r;
            new_pixels.img_[i][j].g = emboss_g;
            new_pixels.img_[i][j].b = emboss_b;
        }
    }
    image_pixels = new_pixels;
}

void Noise::NoiseApply(Image& image, int intensity) {
    Image& image_pixels = image;
    Image new_pixels = image_pixels;
    for (size_t i = 0; i < image_pixels.info_header_.biHeight; ++i) {
        for (size_t j = 0; j < image_pixels.info_header_.biWidth; ++j) {
            BGR pixel = image_pixels.img_[i][j];
            int noise_r = rand() % (2 * intensity + 1) - intensity;
            int noise_g = rand() % (2 * intensity + 1) - intensity;
            int noise_b = rand() % (2 * intensity + 1) - intensity;
            pixel.r = std::clamp(pixel.r + noise_r, 0, constants::MAX_VAL);
            pixel.g = std::clamp(pixel.g + noise_g, 0, constants::MAX_VAL);
            pixel.b = std::clamp(pixel.b + noise_b, 0, constants::MAX_VAL);
            new_pixels.img_[i][j] = pixel;
        }
    }
    image_pixels = new_pixels;
}

void Warholization::WarholizationApply(Image& image) {
    Image& image_pixels = image;
    Image new_pixels = image_pixels;

    std::vector<Image> color_channels;
    for (size_t c = 0; c < 3; ++c) {
        Image color_channel = image;
        for (size_t i = 0; i < image_pixels.info_header_.biHeight; ++i) {
            for (size_t j = 0; j < image_pixels.info_header_.biWidth; ++j) {
                BGR pixel = image_pixels.img_[i][j];
                int color = (c == 0) ? pixel.r : (c == 1) ? pixel.g : pixel.b;
                color_channel.img_[i][j] = BGR(color, color, color);
            }
        }
        color_channels.push_back(color_channel);
    }

    std::vector<BGR> colors = {BGR(constants::MAX_VAL, 0, 0), BGR(0, constants::MAX_VAL, 0),
                               BGR(0, 0, constants::MAX_VAL)};
    for (size_t c = 0; c < 3; ++c) {
        Image color_channel = color_channels[c];
        for (size_t i = 0; i < color_channel.info_header_.biHeight; ++i) {
            for (size_t j = 0; j < color_channel.info_header_.biWidth; ++j) {
                BGR pixel = color_channel.img_[i][j];
                if (pixel.r > constants::MED_VAL) {
                    new_pixels.img_[i][j] = colors[c];
                }
            }
        }
    }
    image_pixels = new_pixels;
}

void Twirl::TwirlApply(Image& image, int radius, double angle) {
    Image& image_pixels = image;
    Image new_pixels = image_pixels;
    double mid_x = image_pixels.info_header_.biWidth / 2;
    double mid_y = image_pixels.info_header_.biHeight / 2;
    BGR current_pixel;
    for (size_t i = 0; i < image_pixels.info_header_.biHeight; ++i) {
        for (size_t j = 0; j < image_pixels.info_header_.biWidth; ++j) {
            double dx = static_cast<double>(j) - mid_x;
            double dy = static_cast<double>(i) - mid_y;
            double dist = sqrt(dx * dx + dy * dy);
            if (dist < radius) {
                double angle2 = angle * (1 - dist / radius);
                double tx = dx * cos(angle2) - dy * sin(angle2);
                double ty = dx * sin(angle2) + dy * cos(angle2);
                int src_x = static_cast<int>(round(tx + mid_x));
                int src_y = static_cast<int>(round(ty + mid_y));
                if (src_x >= 0 && src_x < image_pixels.info_header_.biWidth && src_y >= 0 &&
                    src_y < image_pixels.info_header_.biHeight) {
                    current_pixel = image_pixels.img_[src_y][src_x];
                } else {
                    current_pixel = {0, 0, 0};
                }
            } else {
                current_pixel = image_pixels.img_[i][j];
            }
            new_pixels.img_[i][j] = current_pixel;
        }
    }
    image_pixels = new_pixels;
}