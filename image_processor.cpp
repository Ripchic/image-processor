#include <iostream>
#include "parsing.h"
#include "bmp.h"
#include "filters.h"

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout
            << "Справка по пользованию приложением:\n{имя программы} {путь к входному файлу} {путь к выходному "
               "файлу} [-{имя фильтра 1} [параметр "
               "фильтра 1] "
               "[параметр фильтра 2] ...] [-{имя фильтра 2} [параметр фильтра 1] [параметр фильтра 2] ...] ...\n "
               "Cписок доступных фильтрав:\n -neg, -gs, -crop _ _, -sharp, blur _, -edge _, -pixelate _, -emboss, "
               "-noise _, -warholization, -twirl _ _";
    } else if (argc < 4) {
        std::cout << "Wrong input data\n";
        return 0;
    } else {
        try {
            const ParsedInput parsed_input = ParsedInput::Parse(argc, argv);
            Image img(parsed_input.InputFile.c_str());
            if (parsed_input.InputFile.empty() || parsed_input.OutputFile.empty()) {
                throw std::runtime_error("input / output file was not presented");
            }
            for (size_t i = 0; i < parsed_input.FilterData.size(); ++i) {
                const std::string filter = parsed_input.FilterData[i].first.c_str();
                if (filter == "-crop") {
                    Crop image;
                    try {
                        if (parsed_input.FilterData[i].second.size() > 2) {
                            throw std::out_of_range("too much parameters");
                        }
                        image.CropApply(img, static_cast<int>(parsed_input.FilterData[i].second.at(0)),
                                        static_cast<int>(parsed_input.FilterData[i].second.at(1)));
                    } catch (std::out_of_range& e) {
                        std::cerr << "wrong -crop parameter\n" << e.what();
                        return -1;
                    } catch (...) {
                        std::cerr << "-crop apply error\n";
                        return -1;
                    }
                } else if (filter == "-gs") {
                    GrayScale image;
                    try {
                        image.GrayScaleApply(img);
                    } catch (...) {
                        std::cerr << "-gs apply error\n";
                        return -1;
                    }
                } else if (filter == "-neg") {
                    Negative image;
                    try {
                        image.NegativeApply(img);
                    } catch (...) {
                        std::cerr << "-neg apply error\n";
                        return -1;
                    }
                } else if (filter == "-sharp") {
                    Sharpening image;
                    try {
                        image.SharpeningApply(img);
                    } catch (...) {
                        std::cerr << "-sharp apply error\n";
                        return -1;
                    }
                } else if (filter == "-edge") {
                    EdgeDetection image;
                    try {
                        if (parsed_input.FilterData[i].second.size() > 1) {
                            throw std::out_of_range("too much parameters");
                        }
                        image.EdgeDetectionApply(img, static_cast<float>(parsed_input.FilterData[i].second.at(0)));
                    } catch (std::out_of_range& e) {
                        std::cerr << "wrong -edge parameter\n" << e.what();
                        return -1;
                    } catch (...) {
                        std::cerr << "-edge apply error\n";
                        return -1;
                    }
                } else if (filter == "-blur") {
                    Gaussianblur image;
                    try {
                        if (parsed_input.FilterData[i].second.size() > 1) {
                            throw std::out_of_range("too much parameters");
                        }
                        image.GaussianBlurApply(img, parsed_input.FilterData[i].second.at(0));
                    } catch (std::out_of_range& e) {
                        std::cerr << "wrong -blur parameter\n" << e.what();
                        return -1;
                    } catch (...) {
                        std::cerr << "-blur apply error\n";
                        return -1;
                    }
                } else if (filter == "-pixelate") {
                    Pixelate image;
                    try {
                        if (parsed_input.FilterData[i].second.size() > 1) {
                            throw std::out_of_range("too much parameters");
                        }
                        image.PixelateApply(img, static_cast<int>(parsed_input.FilterData[i].second.at(0)));
                    } catch (std::out_of_range& e) {
                        std::cerr << "wrong -pixelate parameter\n" << e.what();
                        return -1;
                    } catch (...) {
                        std::cerr << "-pixelate apply error\n";
                        return -1;
                    }
                } else if (filter == "-emboss") {
                    Emboss image;
                    try {
                        image.EmbossApply(img);
                    } catch (...) {
                        std::cerr << "-emboss apply error\n";
                        return -1;
                    }
                } else if (filter == "-noise") {
                    Noise image;
                    try {
                        if (parsed_input.FilterData[i].second.size() > 1) {
                            throw std::out_of_range("too much parameters");
                        }
                        image.NoiseApply(img, static_cast<int>(parsed_input.FilterData[i].second.at(0)));
                    } catch (std::out_of_range& e) {
                        std::cerr << "wrong -noise parameter\n" << e.what();
                        return -1;
                    } catch (...) {
                        std::cerr << "-noise apply error\n";
                        return -1;
                    }
                } else if (filter == "-warholization") {
                    Warholization image;
                    try {
                        image.WarholizationApply(img);
                    } catch (...) {
                        std::cerr << "-warholization apply error\n";
                        return -1;
                    }
                } else if (filter == "-twirl") {
                    Twirl image;
                    try {
                        if (parsed_input.FilterData[i].second.size() > 2) {
                            throw std::out_of_range("too much parameters");
                        }
                        image.TwirlApply(img, static_cast<int>(parsed_input.FilterData[i].second.at(0)),
                                         (parsed_input.FilterData[i].second.at(1)));
                    } catch (std::out_of_range& e) {
                        std::cerr << "wrong -twirl parameter\n" << e.what();
                        return -1;
                    } catch (...) {
                        std::cerr << "-twirl apply error\n";
                        return -1;
                    }
                } else {
                    std::cout << "Wrong filter\n";
                }
            }
            img.Save(parsed_input.OutputFile.c_str());
            return 0;
        } catch (std::runtime_error& e) {
            std::cerr << e.what();
            return -1;
        } catch (...) {
            std::cerr << "Please try again later\n";
            return -1;
        }
    }
}