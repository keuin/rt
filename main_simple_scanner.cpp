//
// Created by Keuin on 2022/4/11.
//

// include M_PI from cmath when using MSVC
#define _USE_MATH_DEFINES

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <string>
#include <cmath>

#include "vec.h"
#include "timer.h"
#include "viewport.h"
#include "hitlist.h"
#include "sphere.h"
#include "aa.h"
#include "material_diffusive.h"
#include "material_reflective.h"
#include "material_dielectric.h"

// Select the scene to render
//#define SCENE_DIFFUSE
//#define SCENE_REFLECT
//#define SCENE_DIALECT
//#define SCENE_FOV
//#define SCENE_FREECAM
#define SCENE_DOF

#ifdef SCENE_FOV
#define NO_DEFAULT_CAM
#endif

#ifdef SCENE_FREECAM
#define NO_DEFAULT_CAM
#endif

#ifdef SCENE_DOF
#define NO_DEFAULT_CAM
#endif

static constexpr uint64_t default_diffuse_seed = 123456789012345678ULL;

// T: color depth, V: pos
template<typename T, typename V>
void generate_image(uint16_t image_width, uint16_t image_height, double viewport_width, double focal_length,
                    double sphere_z, double sphere_r, unsigned samples,
                    double aperture, double focus_dist,
                    const std::string &caption = "", unsigned caption_scale = 1) {
    std::cerr << "Aperture: " << aperture << std::endl;
    if (samples == 1) {
        std::cerr << "Antialiasing is disabled." << std::endl;
    } else {
        std::cerr << "Antialiasing Samples: " << samples << std::endl;
    }
    std::cerr << "Initializing context..." << std::endl;
    double r = 1.0 * image_width / image_height;
    hitlist world;

#ifndef NO_DEFAULT_CAM
    ////////////////
    // noaa rendering
    bias_ctx no_bias{};
    basic_viewport<T, V> vp_noaa{
            vec3<V>::zero(), // camera position as the coordinate origin
            vec3d{0, 0, -focal_length},
            image_width,
            image_height,
            viewport_width / 2.0,
            ((double) image_height / image_width) * viewport_width / 2.0,
            aperture,
            focus_dist,
            world
    };
    ////////////////

    ////////////////
    // aa rendering
    aa_viewport<T, V> vp_aa{
            vec3<V>::zero(), // camera position as the coordinate origin
            vec3d{0, 0, -focal_length},
            image_width, image_height,
            viewport_width / 2.0, ((double) image_height / image_width) * viewport_width / 2.0,
            aperture, focus_dist,
            world, samples
    };
    ////////////////
#endif
#ifdef SCENE_FOV
    // This scene needs custom viewport setting
    const auto fov_h = 2 * asin((double)1.0 / 3.0);
    ////////////////
    // noaa rendering
    bias_ctx no_bias{};
    basic_viewport<T, V> vp_noaa{
            {0, 0, 1}, // camera position as the coordinate origin
            {0, 0, 0},
            image_width, image_height,
            fov_h,
            aperture,
            focus_dist,
            world
    };
    ////////////////

    ////////////////
    // aa rendering
    aa_viewport<T, V> vp_aa{
            {0, 0, 1}, // camera position as the coordinate origin
            {0, 0, 0},
            image_width, image_height,
            fov_h,
            aperture,
            focus_dist,
            world, samples
    };
    ////////////////

    material_diffuse_lambertian materi{0.5};
    world.add_object(std::make_shared<sphere>(vec3d{0, 0, -2}, 1, materi));
#endif
#ifdef SCENE_FREECAM
    // This scene needs custom viewport setting
    const auto fov_h = 121.28449291441746 * (M_PI / 180.0);
    ////////////////
    // noaa rendering
    bias_ctx no_bias{};
    basic_viewport<T, V> vp_noaa{
            {-2,2,1}, // camera position as the coordinate origin
            {0,0,-1},
            image_width, image_height,
            fov_h,
            aperture,
            focus_dist,
            world
    };
    ////////////////

    ////////////////
    // aa rendering
    aa_viewport<T, V> vp_aa{
            {-2,2,1}, // camera position as the coordinate origin
            {0,0,-1},
            image_width, image_height,
            fov_h,
            aperture,
            focus_dist,
            world, samples
    };
    ////////////////

    material_diffuse_lambertian m_ground{{0.8, 0.8, 0.0}};
    material_diffuse_lambertian m_ball_center{{0.7, 0.3, 0.3}};
    material_dielectric m_ball_left{1.5};
    material_reflective m_ball_right{{0.8, 0.6, 0.2}};
    // the earth
    world.add_object(std::make_shared<sphere>(vec3d{0.0, -100.5, -1.0}, 100.0, m_ground));
    // three balls
    world.add_object(std::make_shared<sphere>(vec3d{-1.0, 0.0, -1.0}, 0.5, m_ball_left));
    world.add_object(std::make_shared<sphere>(vec3d{-1.0, 0.0, -1.0}, -0.45, m_ball_left));
    world.add_object(std::make_shared<sphere>(vec3d{0.0, 0.0, -1.0}, 0.5, m_ball_center));
    world.add_object(std::make_shared<sphere>(vec3d{1.0, 0.0, -1.0}, 0.5, m_ball_right));
#endif
#ifdef SCENE_DOF
    focus_dist = 1.7320508075688772;
    // This scene needs custom viewport setting
    const auto fov_h = 121.28449291441746 * (M_PI / 180.0);
    ////////////////
    // noaa rendering
    bias_ctx no_bias{};
    basic_viewport<T, V> vp_noaa{
            {-2, 2, 1}, // camera position as the coordinate origin
            {-1, 1, 0},
            image_width, image_height,
            fov_h,
            aperture,
            focus_dist,
            world
    };
    ////////////////

    ////////////////
    // aa rendering
    aa_viewport<T, V> vp_aa{
            {-2, 2, 1}, // camera position as the coordinate origin
            {-1, 1, 0},
            image_width, image_height,
            fov_h,
            aperture,
            focus_dist,
            world, samples
    };
    ////////////////

    material_diffuse_lambertian m_ground{{0.8, 0.8, 0.0}};
    material_diffuse_lambertian m_ball_center{{0.7, 0.3, 0.3}};
    material_dielectric m_ball_left{1.5};
    material_reflective m_ball_right{{0.8, 0.6, 0.2}};
    // the earth
    world.add_object(std::make_shared<sphere>(vec3d{0.0, -100.5, -1.0}, 100.0, m_ground));
    // three balls
    world.add_object(std::make_shared<sphere>(vec3d{-1.0, 0.0, -1.0}, 0.5, m_ball_left));
    world.add_object(std::make_shared<sphere>(vec3d{-1.0, 0.0, -1.0}, -0.45, m_ball_left));
    world.add_object(std::make_shared<sphere>(vec3d{0.0, 0.0, -1.0}, 0.5, m_ball_center));
    world.add_object(std::make_shared<sphere>(vec3d{1.0, 0.0, -1.0}, 0.5, m_ball_right));
#endif
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
#ifdef SCENE_DIALECT
    material_diffuse_lambertian m_ground{{0.8, 0.8, 0.0}};
    material_diffuse_lambertian m_ball_center{{0.7, 0.3, 0.3}};
    material_dielectric m_ball_left{1.5};
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
    bokeh_ctx bokeh{12345678ULL};
    tm.start_measure();
    auto image = ((samples == 1) ? vp_noaa.render(default_diffuse_seed, no_bias, bokeh) : vp_aa.render());
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
}

int main(int argc, char **argv) {
    if (argc != 10 && argc != 11) {
        printf("Usage: %s <image_width> <image_height> <viewport_width> <focal_length> <sphere_z> <sphere_r> <aperture> <focus_dist> <samples> [<caption>]\n",
               argv[0]);
        return 0;
    }
#ifndef NDEBUG
    std::cerr << "Notice: assertion is enabled." << std::endl;
#else
    std::cerr << "Notice: assertion is disabled." << std::endl;
#endif
    std::string iw{argv[1]}, ih{argv[2]}, vw{argv[3]}, fl{argv[4]},
            sz{argv[5]}, sr{argv[6]}, aper{argv[7]}, fd{argv[8]}, sp{argv[9]}, cap{};
    if (argc == 11) {
        // with caption
        cap = std::string{argv[10]};
    }
    const auto image_width = std::stoul(iw);
    generate_image<uint16_t, double>(image_width, std::stoul(ih),
                                     std::stod(vw), std::stod(fl),
                                     std::stod(sz), std::stod(sr),
                                     std::stoul(sp), std::stod(aper),
                                     std::stod(fd),
                                     cap, std::max((int) (1.0 * image_width * 0.010 / 8), 1));
}