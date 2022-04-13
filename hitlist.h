//
// Created by Keuin on 2022/4/12.
//

#ifndef RT_HITLIST_H
#define RT_HITLIST_H

#include "viewport.h"
#include "timer.h"
#include "bitmap.h"
#include "ray.h"
#include "vec.h"
#include "object.h"
#include <cstdlib>
#include <memory>
#include <limits>
#include <vector>
#include <iostream>
#include <cstdint>

//#define T_SIMPLE_COLOR
//#define T_NORM_VISUAL
#define T_DIFFUSE

// A world, T is color depth
template<typename T>
class hitlist {
    std::vector<std::shared_ptr<object>> objects;

public:
    hitlist() = default;

    hitlist(hitlist &other) = delete; // do not copy the world

    // Add an object to the world.
    void add_object(std::shared_ptr<object> &&obj) {
        objects.push_back(std::move(obj));
    }

    // Given a ray, compute the color.
    pixel<T> color(ray3d r, random_uv_gen_3d &ruvg, uint_fast32_t max_recursion_depth = 64) const {
        double decay = 1;
        while (max_recursion_depth-- > 0) {
            // Detect hits
            bool hit = false;
            double hit_t = std::numeric_limits<double>::infinity();
            std::shared_ptr<object> hit_obj;
            // Check the nearest object we hit
            for (const auto &obj: objects) {
                double t_;
                if (obj->hit(r, t_, 0.0) && t_ < hit_t) {
                    hit = true;
                    hit_t = t_;
                    hit_obj = obj;
                }
            }
            if (hit) {
#ifdef T_SIMPLE_COLOR
                // simply returns color of the object
                return hit_obj->color();
#endif
#ifdef T_NORM_VISUAL
                // normal vector on hit point
                const auto nv = hit_obj->normal_vector(r.at(hit_t));
                // visualize normal vector at hit point
                return pixel<T>::from_normalized(nv);
#endif
#ifdef T_DIFFUSE
                const auto hit_point = r.at(hit_t); // hit point, on the surface
                auto nv = hit_obj->normal_vector(hit_point);
                if (dot(nv, r.direction()) > 0) return pixel<T>::black(); // discard rays from inner (or invert nv)
                vec3d diffuse_target = hit_point + nv + ruvg();
                decay *= 0.5; // lose 50% light when diffused
                r = ray3d{hit_point, diffuse_target - hit_point}; // the new diffused ray we trace on
                continue;
#endif
            }

            // Does not hit anything. Get background color (infinity)
            const auto u = (r.direction().y + 1.0) * 0.5;
            const auto c = mix(
                    pixel<T>::from_normalized(1.0, 1.0, 1.0),
                    pixel<T>::from_normalized(0.5, 0.7, 1.0),
                    1.0 - u,
                    u
            );
#ifdef T_DIFFUSE
            return decay * c;
#else
            return c;
#endif
        }
        return pixel<T>::black(); // reached recursion time limit, very little light
    }
};

using hitlist8b = hitlist<uint8_t>;

#endif //RT_HITLIST_H
