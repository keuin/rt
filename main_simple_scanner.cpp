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
#include "material_diffusive.h"
#include "material_reflective.h"

// Select the scene to render
//#define SCENE_DIFFUSE
#define SCENE_REFLECT

// T: intermediate color depth
template<typename T>
void generate_image(uint16_t image_width, uint16_t image_height, double viewport_width, double focal_length,
                    double sphere_z, double sphere_r, unsigned samples, const std::string &caption = "",
                    unsigned caption_scale = 1) {
    if (samples == 1) {
        std::cerr << "Antialiasing is disabled." << std::endl;
    } else {
        std::cerr << "Antialiasing Samples: " << samples << std::endl;
    }
    std::cerr << "Initializing context..." << std::endl;
    double r = 1.0 * image_width / image_height;
    viewport<T> *vp;
    if (samples == 1) {
        vp = new basic_viewport<T>{viewport_width, viewport_width / r, vec3d{0, 0, -focal_length}};
    } else {
        vp = new aa_viewport<T>{viewport_width, viewport_width / r, vec3d{0, 0, -focal_length}, samples};
    }
    hitlist world;
#ifdef SCENE_DIFFUSE
    material_diffuse_lambertian materi{0.5};
    world.add_object(std::make_shared<sphere>(
            vec3d{0, -100.5, -1},
            100, materi)); // the earth
    world.add_object(std::make_shared<sphere>(vec3d{0, 0, sphere_z}, sphere_r, materi));
#endif
#ifdef SCENE_REFLECT
    material_diffuse_lambertian m_ground{{0.8, 0.8, 0.0}};
    material_diffuse_lambertian m_ball_center{{0.7, 0.3, 0.3}};
    material_fuzzy_reflective m_ball_left{{0.8, 0.8, 0.8}, 1.0};
    material_reflective m_ball_right{{0.8, 0.6, 0.2}};
    // the earth
    world.add_object(std::make_shared<sphere>(vec3d{0.0, -100.5, -1.0}, 100.0, m_ground));
    // three balls
    world.add_object(std::make_shared<sphere>(vec3d{-1.0, 0.0, -1.0}, 0.5, m_ball_left));
    world.add_object(std::make_shared<sphere>(vec3d{0.0, 0.0, -1.0}, 0.5, m_ball_center));
    world.add_object(std::make_shared<sphere>(vec3d{1.0, 0.0, -1.0}, 0.5, m_ball_right));
#endif
    timer tm;
    std::cerr << "Rendering..." << std::endl;
    tm.start_measure();
    auto image = vp->render(world, vec3d::zero(),
                            image_width, image_height); // camera position as the coordinate origin
    tm.stop_measure();
    std::cerr << "Applying gamma2..." << std::endl;
    tm.start_measure();
    image = image.gamma2(); // gamma correction
    tm.stop_measure();
    if (!caption.empty()) {
        image.print(caption,
                    pixel<T>::from_normalized(1.0, 0.0, 0.0),
                    10, 10, text_policy::newline, caption_scale, 0.8);
    }
    const auto image8b = bitmap8b::from<T>(image);
    if (!std::getenv("NOPRINT")) {
        image8b.write_plain_ppm(std::cout);
    } else {
        std::cerr << "NOPRINT is defined. PPM Image won't be printed." << std::endl;
    }
    delete vp;
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
    generate_image<uint16_t>(image_width, std::stoul(ih),
                             std::stod(vw), std::stod(fl),
                             std::stod(sz), std::stod(sr),
                             std::stoul(sp), cap,
                             std::max((int) (1.0 * image_width * 0.010 / 8), 1));
}