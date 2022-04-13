//
// Created by Keuin on 2022/4/11.
//

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>

#include "vec.h"
#include "timer.h"
#include "viewport.h"
#include "hitlist.h"
#include "sphere.h"
#include "aa.h"

#define DEMO_BALL

void generate_image(uint16_t image_width, uint16_t image_height, double viewport_width, double focal_length,
                    double sphere_z, double sphere_r, unsigned samples, const std::string &caption = "",
                    unsigned caption_scale = 1) {
    if (samples == 1) {
        std::cerr << "Antialiasing is disabled." << std::endl;
    } else {
        std::cerr << "Antialiasing Samples: " << samples << std::endl;
    }
    double r = 1.0 * image_width / image_height;
    aa_viewport8b vp{viewport_width, viewport_width / r, vec3d{0, 0, -focal_length}, samples};
    hitlist8b world;
    world.add_object(std::make_shared<sphere>(
            vec3d{0, -100.5, -1},
            100)); // the earth
    world.add_object(std::make_shared<sphere>(vec3d{0, 0, sphere_z}, sphere_r));
    timer tm;
    tm.start_measure();
    auto image = vp.render(world, vec3d::zero(),
                           image_width, image_height); // camera position as the coordinate origin
    tm.stop_measure();
    if (!caption.empty()) {
        image.print(caption,
                    pixel8b::from_normalized(1.0, 0.0, 0.0),
                    10, 10, caption_scale, 0.8);
    }
    if (!std::getenv("NOPRINT")) {
        image.write_plain_ppm(std::cout);
    } else {
        std::cerr << "NOPRINT is defined. PPM Image won't be printed." << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc != 8 && argc != 9) {
        printf("Usage: %s <image_width> <image_height> <viewport_width> <focal_length> <sphere_z> <sphere_r> <samples> [<caption>]\n",
               argv[0]);
        return 0;
    }
#ifndef NDEBUG
    std::cerr << "Notice: assertion is enabled." << std::endl;
#endif
    std::string iw{argv[1]}, ih{argv[2]}, vw{argv[3]}, fl{argv[4]},
            sz{argv[5]}, sr{argv[6]}, sp{argv[7]}, cap{};
    if (argc == 9) {
        // with caption
        cap = std::string{argv[8]};
    }
    const auto image_width = std::stoul(iw);
    generate_image(image_width, std::stoul(ih),
                   std::stod(vw), std::stod(fl),
                   std::stod(sz), std::stod(sr),
                   std::stoul(sp), cap,
                   std::max((int) (1.0 * image_width * 0.015 / 8), 1));
}