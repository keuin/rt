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
#include <cmath>

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

// Generate a circle of confusion of circle shape. (can be extended to any shape in the future)
class bokeh_ctx {
    bool enabled;
    std::mt19937_64 mt;
    std::uniform_real_distribution<double> uni{-1.0, 1.0};

public:
    bokeh_ctx() : enabled{false} {}

    bokeh_ctx(uint64_t seed) : enabled{true}, mt{seed} {}

    void operator()(double &x, double &y) {
        if (!enabled) {
            x = 0;
            y = 0;
            return;
        }
        double x_, y_;
        do {
            x_ = uni(mt);
            y_ = uni(mt);
        } while (x_ * x_ + y_ * y_ >= 1);
        x = x_;
        y = y_;
    }
};

// TODO rename to camera
// Single sampled viewport which supports bias sampling
// U: color depth, V: pos
template<typename U, typename V>
class basic_viewport {
    vec3<V> cxyz; // coordinate of the focus point
    vec3<V> screen_center;
//    double pitch; // TODO implement
//    double yaw; // TODO implement
    uint32_t image_width; // how many pixels every row has
    uint32_t image_height; // how many pixels every column has
    V screen_hw; // determined if screen_height is known
    V screen_hh; // determined if screen_width is known
//    double fov_h; // horizontal FOV, determined if screen_width or screen_height is known
//    double focus_length; // distance between the focus point and the image screen
    hitlist &world;
    vec3<V> vup{0, 1, 0}; // vector determine the camera rotating
    double aperture; // radius ratio of the aperture
    double focus_dist;

    inline void check_vup() const {
        // vup must not be parallel with screen_center-cxyz
        assert(!vup.parallel(screen_center - cxyz));
    }

public:

    basic_viewport(const vec3<V> &cxyz, const vec3<V> &screen_center,
                   uint32_t image_width, uint32_t image_height,
                   double fov_h, double aperture, double focus_dist, hitlist &world) :
            cxyz{cxyz}, screen_center{screen_center}, image_width{image_width}, image_height{image_height},
            screen_hw{(cxyz - screen_center).norm() * tan((double) fov_h / 2.0)},
            screen_hh{screen_hw * ((double) image_height / image_width)},
            world{world},
            aperture{aperture}, focus_dist{focus_dist} {
        check_vup();
    }

    basic_viewport(const vec3<V> &cxyz, const vec3<V> &screen_center,
                   uint32_t image_width, uint32_t image_height,
                   double screen_hw, double screen_hh,
                   double aperture, double focus_dist,
                   hitlist &world) :
            cxyz{cxyz}, screen_center{screen_center}, image_width{image_width}, image_height{image_height},
            screen_hw{screen_hw},
            screen_hh{screen_hh},
            world{world},
            aperture{aperture}, focus_dist{focus_dist} {
        assert(std::abs(1.0 * image_width / image_height - 1.0 * screen_hw / screen_hh) < 1e-8);
        check_vup();
    }

    /**
     * Generate the image seen on given viewpoint.
     * @param bx bias on x axis (0.0 <= bx < 1.0)
     * @param by bias on y axis (0.0 <= by < 1.0)
     * @return
     */
    bitmap<U> render(uint64_t diffuse_seed, bias_ctx &bias, bokeh_ctx &bokeh
            /* by putting thread-specific parameters in call argument list, make users convenient*/) const {
        // The implementation keep all mutable state in local stack,
        // keeping the class immutable and thread-safe.
        bitmap<U> image{image_width, image_height};
        random_uv_gen_3d ruvg{diffuse_seed};
        V bx, by;
        const auto r = screen_center - cxyz;
        const int img_hw = image_width / 2, img_hh = image_height / 2;
        // screen plane is determined by coord system x`Vy`, where V is screen_center
        // for variable name we let u := x`, v := y`
        const auto u0 = cross(r, vup).unit_vec(), v0 = cross(u0, r).unit_vec();
        const auto u = u0 * screen_hw, v = v0 * screen_hh;
        assert(dot(r, u) < 1e-8);
        assert(dot(r, v) < 1e-8);
        assert(dot(u, v) < 1e-8);
        const V pof_scale = (V) 1.0 + (V) focus_dist / r.norm();
        // iterate over every pixel on the image
        for (int j = -img_hh + 1; j <= img_hh; ++j) { // axis y, transformation is needed
            for (int i = -img_hw; i < img_hw; ++i) { // axis x
                bias(bx, by); // get a random bias (bx, by) for sub-pixel sampling
                assert(0 <= bx);
                assert(0 <= by);
                assert(bx < 1.0);
                assert(by < 1.0);
                const auto off_u = (1.0 * i + bx) / img_hw;
                const auto off_v = (1.0 * j + by) / img_hh;
                const auto off = off_u * u + off_v * v; // offset on screen plane
                const auto dir = r + off; // direction vector from camera to current pixel on screen
                const auto dir_pof = dir * pof_scale; // difference from camera to point on focus plane
                const auto pof = cxyz + dir_pof; // point on focus plane, the destination
                double bokeh_u, bokeh_v;
                bokeh(bokeh_u, bokeh_v);
                const auto source = cxyz + (aperture * bokeh_u) * u0 + (aperture * bokeh_v) * v0;
                ray3d ray{source, pof - source}; // from camera to pixel (on the viewport)
                const auto pixel = world.color<U>(ray, ruvg);
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
