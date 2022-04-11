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
    uint16_t width, height;
    vec3d center;
    bitmap8b bitmap_{width, height};

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

    viewport(uint16_t width, uint16_t height, vec3d viewport_center) :
            width(width), height(height), center(viewport_center) {}

    void render(vec3d viewpoint) {
        const auto r = center - viewpoint;
        const int half_width = width / 2, half_height = height / 2;
        for (int j = -half_height; j < half_height; ++j) {
            for (int i = -half_width; i < half_width; ++i) {
                const auto dir = r + vec3d{static_cast<double>(i), static_cast<double>(j), 0};
                const ray3d ray{viewpoint, dir}; // from camera to pixel (on the viewport)
                const auto pixel = color(ray);
                bitmap_.set(i + half_width, j + half_height, pixel);
            }
        }
    }

    const bitmap8b &bitmap() const {
        return bitmap_;
    }
};

int main() {
    viewport vp{1920, 1080, vec3d{0, 0, -100}};
    timer tm;
    tm.start_measure();
    vp.render(vec3d{0, 0, 0}); // camera position as the coordinate origin
    tm.stop_measure();
    vp.bitmap().write_plain_ppm(std::cout);
}