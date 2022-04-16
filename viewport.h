//
// Created by Keuin on 2022/4/12.
//

#ifndef RT_VIEWPORT_H
#define RT_VIEWPORT_H

#include "timer.h"
#include "bitmap.h"
#include "ray.h"
#include "vec.h"
#include "hitlist.h"
#include "tracelog.h"
#include <cstdlib>
#include <memory>
#include <limits>
#include <vector>
#include <iostream>
#include <cstdint>
#include <random>

// bias context, used for placing sub-pixels
class bias_ctx {
    bool enabled; // put all together, eliminating a virtual function call
    std::mt19937_64 mt;
    std::uniform_real_distribution<double> uni{0.0, 1.0};

public:
    bias_ctx() : enabled(false) {}

    bias_ctx(uint64_t seed) : enabled(true), mt(std::mt19937_64{seed}) {}

    void operator()(double &bx, double &by) {
        if (enabled) {
            bx = uni(mt);
            by = uni(mt);
        } else {
            bx = 0.0;
            by = 0.0;
        }
    }
};

template<typename T>
class viewport {
public:
    virtual bitmap<T> render(const hitlist &world, vec3d viewpoint, uint16_t image_width, uint16_t image_height) = 0;

    virtual ~viewport() = default;
};

// Single sampled viewport which supports bias sampling
template<typename T>
class basic_viewport : public viewport<T> {
    const double half_width, half_height; // viewport size
    const vec3d center; // coordinate of the viewport center point

public:
    basic_viewport() = delete;

    basic_viewport(double width, double height, vec3d viewport_center) :
            half_width(width / 2.0), half_height(height / 2.0), center(viewport_center) {}

    virtual bitmap<T>
    render(const hitlist &world, vec3d viewpoint, uint16_t image_width, uint16_t image_height) override {
        bias_ctx bc{};
        static constexpr uint64_t default_diffuse_seed = 123456789012345678ULL;
        return render(world, viewpoint, image_width, image_height, bc, default_diffuse_seed);
    }

    /**
     * Generate the image seen on given viewpoint.
     * @param bx bias on x axis (0.0 <= bx < 1.0)
     * @param by bias on y axis (0.0 <= by < 1.0)
     * @return
     */
    virtual bitmap<T> render(const hitlist &world, vec3d viewpoint,
                             uint16_t image_width, uint16_t image_height,
                             bias_ctx &bias, uint64_t diffuse_seed) const {
        bitmap<T> image{image_width, image_height};
        random_uv_gen_3d ruvg{diffuse_seed};
        double bx, by;
        const auto r = center - viewpoint;
        const int img_hw = image_width / 2, img_hh = image_height / 2;
        // iterate over every pixel on the image
        for (int j = -img_hh + 1; j <= img_hh; ++j) { // axis y, transformation is needed
            for (int i = -img_hw; i < img_hw; ++i) { // axis x
                bias(bx, by); // get a random bias (bx, by) for sub-pixel sampling
                assert(0 <= bx);
                assert(0 <= by);
                assert(bx < 1.0);
                assert(by < 1.0);
                const vec3d off{
                        .x=(1.0 * i + bx) / img_hw * half_width,
                        .y=(1.0 * j + by) / img_hh * half_height,
                        .z=0.0
                }; // offset on screen plane
                const auto dir = r + off; // direction vector from camera to current pixel on screen
                ray3d ray{viewpoint, dir}; // from camera to pixel (on the viewport)
                const auto pixel = world.color<T>(ray, ruvg);
                const auto x_ = i + img_hw, y_ = -j + img_hh;
                image.set(x_, y_, pixel);

#ifdef LOG_TRACE
                const auto ret = pixel;
                const auto ret8b = pixel8b::from(ret);
                const auto ret8bg2 = pixel8b::from(ret.gamma2());
                TRACELOG(" ^  apply to pixel: (%d, %d), color: [%f, %f, %f] (8bit: [%d, %d, %d], 8bit-gamma2: [%d, %d, %d])\n",
                         x_, y_,
                         (double) ret.r, (double) ret.g, (double) ret.b,
                         ret8b.r, ret8b.g, ret8b.b,
                         ret8bg2.r, ret8bg2.g, ret8bg2.b);
#endif
            }
        }
        return image;
    }
};

#endif //RT_VIEWPORT_H
