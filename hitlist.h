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


// A world
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
    pixel8b color(const ray3d &r) const {
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
            // normal vector on hit point
            const auto nv = hit_obj->normal_vector(r.at(hit_t));
//                return obj->color();
            // visualize normal vector at hit point
            return pixel8b::from_normalized(nv);
        }


        // Does not hit anything. Get background color (infinity)
        const auto u = (r.direction().y + 1.0) * 0.5;
        return mix(
                pixel8b::from_normalized(1.0, 1.0, 1.0),
                pixel8b::from_normalized(0.5, 0.7, 1.0),
                1.0 - u,
                u
        );
    }


};

#endif //RT_HITLIST_H
