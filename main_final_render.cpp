//
// Created by Keuin on 2022/4/21.
//

// include M_PI from cmath when using MSVC

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "viewport.h"
#include "aa.h"
#include "bitmap.h"
#include "final_scene.h"
#include "timer.h"
#include <string>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("%s <image_width> <image_height> <sample_per_pixel>\n", argv[0]);
        return 0;
    }
    const bool no_print = std::getenv("NOPRINT") != nullptr;
    if (no_print) {
        std::cerr << "NOPRINT is set. Result image won't be printed to STDOUT." << std::endl;
    }
    std::string s_image_width{argv[1]}, s_image_height{argv[2]}, s_spp{argv[3]};
    uint32_t image_width = std::stoul(s_image_width);
    uint32_t image_height = std::stoul(s_image_height);
    uint32_t spp = std::stoul(s_spp);
    std::cerr << "image size: " << image_width << "*" << image_height << std::endl
              << "spp: " << spp << std::endl;

    final_scene<uint16_t, double> scene{0x793def6344ef29d4};
    timer tm;
    std::cerr << "Start rendering..." << std::endl;
    tm.start_measure();
    const auto image = scene.render(image_width, image_height, spp);
    tm.stop_measure();
    std::cerr << "Apply gamma2 correction..." << std::endl;
    tm.start_measure();
    const auto image2 = image.gamma2();
    tm.stop_measure();
    if (!no_print) image2.write_plain_ppm(std::cout);
}