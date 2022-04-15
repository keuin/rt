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
    template<typename T>
    pixel<T> color(ray3d r, random_uv_gen_3d &ruvg, uint_fast32_t max_recursion_depth = 64) const {
        assert(r.decay().is_one());
        while (max_recursion_depth-- > 0) {
            // Detect hits
            bool hit = false;
            double hit_t = std::numeric_limits<double>::infinity();
            std::shared_ptr<object> hit_obj;
            // Check the nearest object we hit
            for (const auto &obj: objects) {
                double t_;
                // Fix the Shadow Acne problem
                // Some diffused rays starts off from a point on the surface,
                // while hit the surface very quickly at some small t like +-1e-10 or so.
                // This decays the ray by accident, causing black pixels on the output image.
                // We simply drop hits very near the surface
                // (i.e. t is a very small positive number) to solve this problem.
                static constexpr double t_min = 1e-8;
                if (obj->hit(r, t_, t_min) && t_ < hit_t) {
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
                const auto &materi = hit_obj->material();
                if (materi.scatter(r, *hit_obj, hit_t, ruvg)) {
                    continue; // The ray is scatted by an object. Continue processing the scattered ray.
                } else {
                    return pixel<T>::black(); // The ray is absorbed by an object completely. Return black.
                }
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
            return r.hit(c);
#else
            return c;
#endif
        }
        return pixel<T>::black(); // reached recursion time limit, very little light
    }
};

#endif //RT_HITLIST_H
