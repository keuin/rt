//
// Created by Keuin on 2022/4/21.
//

#include "viewport.h"
#include "hitlist.h"
#include "aa.h"
#include "bitmap.h"

#include "material_diffusive.h"
#include "material_reflective.h"
#include "material_dielectric.h"

#include "sphere.h"

#include "timer.h"

#include <random>
#include <memory>
#include <vector>
#include <string>

template<typename T_Color, typename T_Coord>
class final_scene {
    hitlist world;
    std::vector<material *> materials;
public:
    explicit final_scene(uint64_t seed);

    ~final_scene() {
        for (material *item: materials) {
            delete item;
        }
    }

    bitmap<T_Color> render(uint32_t image_width, uint32_t image_height, uint32_t spp) {
        aa_viewport<T_Color, T_Coord> camera{
                {13, 2, 3},
                {6.5, 1, 1.5}, //{0, 0, 0},
                static_cast<uint16_t>(image_width),
                static_cast<uint16_t>(image_height),
                28.0 * (M_PI / 180.0),
                0.025,
                2.432043413435534,
                world,
                spp
        };
        return camera.render();
    }
};

template<typename T_Color, typename T_Coord>
final_scene<T_Color, T_Coord>::final_scene(uint64_t seed) : world{} {
    std::mt19937_64 rand{seed};
    std::uniform_real_distribution<double> uni{0, 1};
    material *m_ball;

    // shared materials
    const auto m_glass = new material_dielectric{1.5};
    materials.push_back(m_glass);

    // earth
    m_ball = new material_diffuse_lambertian{{0.5, 0.5, 0.5}};
    world.add_object(std::make_shared<sphere>(vec3d{0, -1000, 0}, 1000.0, *m_ball));
    materials.push_back(m_ball);

    // add random small balls
    for (int i = -11; i < 11; ++i) {
        for (int j = -11; j < 11; ++j) {
            double rv = uni(rand);
            vec3d center{i + 0.9 * uni(rand), 0.2, j + 0.9 * uni(rand)};

            if ((center - vec3d{4, 0.2, 0}).norm() > 0.9) {

                if (rv < 0.8) {
                    // diffuse
                    const auto albedo = vec3d{
                            uni(rand) * uni(rand),
                            uni(rand) * uni(rand),
                            uni(rand) * uni(rand)
                    };
                    m_ball = new material_diffuse_lambertian{albedo};
                    world.add_object(std::make_shared<sphere>(center, 0.2, *m_ball));
                    materials.push_back(m_ball);
                } else if (rv < 0.95) {
                    // metal
                    auto albedo = vec3d{
                            0.5 * uni(rand) + 0.5,
                            0.5 * uni(rand) + 0.5,
                            0.5 * uni(rand) + 0.5
                    };
                    auto fuzz = 0.5 * uni(rand);
                    m_ball = new material_fuzzy_reflective{albedo, fuzz};
                    world.add_object(std::make_shared<sphere>(center, 0.2, *m_ball));
                    materials.push_back(m_ball);
                } else {
                    // glass
                    world.add_object(std::make_shared<sphere>(center, 0.2, *m_glass));
                }
            }
        }
    }

    // add three big balls
    material *mat;
    mat = new material_dielectric{1.5};
    world.add_object(std::make_shared<sphere>(
            vec3d{0, 1, 0},
            1.0,
            *mat
    ));
    materials.push_back(mat);

    mat = new material_diffuse_lambertian{{0.4, 0.2, 0.1}};
    world.add_object(std::make_shared<sphere>(
            vec3d{-4, 1, 0},
            1.0,
            *mat
    ));
    materials.push_back(mat);

    mat = new material_reflective{{0.7, 0.6, 0.5}}; // not fuzzy
    world.add_object(std::make_shared<sphere>(
            vec3d{4, 1, 0},
            1.0,
            *mat
    ));
    materials.push_back(mat);
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("%s <image_width> <image_height> <sample_per_pixel>\n", argv[0]);
        return 0;
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
    image2.write_plain_ppm(std::cout);
}