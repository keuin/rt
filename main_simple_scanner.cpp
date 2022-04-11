//
// Created by Keuin on 2022/4/11.
//

#include <cstdint>
#include <iostream>

#include "vec.h"
#include "ray.h"
#include "bitmap.h"
#include "timer.h"


class viewport {
    double half_width, half_height; // viewport size
    vec3d center; // coordinate of the viewport center point

    // Given a ray from the camera, generate a color the camera seen on the viewport.
    static pixel8b color(const ray3d &r) {
        const auto u = (r.direction().y + 1.0) * 0.5;
        return mix(
                pixel8b::from_normalized(1.0, 1.0, 1.0),
                pixel8b::from_normalized(0.5, 0.7, 1.0),
                1.0 - u,
                u
        );
    }

public:
    viewport() = delete;

    viewport(double width, double height, vec3d viewport_center) :
            half_width(width / 2.0), half_height(height / 2.0), center(viewport_center) {}

    // Generate the image seen on given viewpoint.
    bitmap8b render(vec3d viewpoint, uint16_t image_width, uint16_t image_height) {
        bitmap8b image{image_width, image_height};
        const auto r = center - viewpoint;
        const int img_hw = image_width / 2, img_hh = image_height / 2;
        // iterate over every pixel on the image
        for (int j = -img_hh; j < img_hh; ++j) { // axis y
            for (int i = -img_hw; i < img_hw; ++i) { // axis x
                const vec3d off{
                        .x=1.0 * i / half_width,
                        .y=1.0 * j / half_height,
                        .z=0.0
                }; // offset on screen plane
                const auto dir = r + off; // direction vector from camera to current pixel on screen
                const ray3d ray{viewpoint, dir}; // from camera to pixel (on the viewport)
                const auto pixel = color(ray);
                image.set(i + img_hw, j + img_hh, pixel);
            }
        }
        return image;
    }
};

void generate_image(uint16_t image_width, uint16_t image_height, double viewport_width, double focal_length) {
    double r = 1.0 * image_width / image_height;
    viewport vp{viewport_width, viewport_width / r, vec3d{0, 0, -focal_length}};
    timer tm;
    tm.start_measure();
    const auto image = vp.render(vec3d::zero(), image_width, image_height); // camera position as the coordinate origin
    tm.stop_measure();
    image.write_plain_ppm(std::cout);
}

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("Usage: %s <image_width> <image_height> <viewport_width> <focal_length>\n", argv[0]);
        return 0;
    }
    std::string iw{argv[1]}, ih{argv[2]}, vw{argv[3]}, fl{argv[4]};
    generate_image(std::stoul(iw), std::stoul(ih),
                   std::stod(vw), std::stod(fl));
}