//
// Created by Keuin on 2022/4/12.
//

#ifndef RT_SPHERE_H
#define RT_SPHERE_H

#include "object.h"
#include "hitlist.h"
#include "viewport.h"
#include "timer.h"
#include "bitmap.h"
#include "ray.h"
#include "vec.h"
#include <cstdlib>
#include <memory>
#include <limits>
#include <vector>
#include <iostream>
#include <cstdint>

class sphere : public object {
    vec3d center;
    double radius;

public:
    sphere() = delete;

    sphere(const vec3d &center, double radius) : center(center), radius(radius) {}

    ~sphere() override = default;

    vec3d normal_vector(const vec3d &where) const override {
        // We assume the point is on surface, speeding up the normalization
        return (where - center) / radius;
    }

    bool hit(const ray3d &r, double &t, double t1, double t2) const override {
        // Ray: {Source, Direction, time}
        // Sphere: {Center, radius}
        // sphere hit formula: |Source + Direction * time - Center| = radius
        // |(Sx + Dx * t - Cx, Sy + Dy * t - Cy, Sz + Dz * t - Cz)| = radius

        const auto c2s = r.source() - center; // center to source
        // A = D dot D
        const double a = r.direction().mod2();
        // H = (S - C) dot D
        const auto h = dot(c2s, r.direction());
        // B = 2H ( not used in our optimized routine )
        // C = (S - C) dot (S - C) - radius^2
        const double c = c2s.mod2() - radius * radius;
        // 4delta = H^2 - AC
        // delta_q = H^2 - AC (quarter delta)
        const double delta_q = h * h - a * c;

        bool hit = false;
        if (delta_q >= 0) {
            // return the root in range [t1, t2]
            // t = ( -H +- sqrt{ delta_q } ) / A
            double root;
            root = (-h - sqrt(delta_q)) / a;
            if (root >= t1 && root <= t2) {
                hit = true;
                t = root;
            } else {
                root = (-h + sqrt(delta_q)) / a;
                if (root >= t1 && root <= t2) {
                    hit = true;
                    t = root;
                }
            }
        }
        return hit;
    }

    pixel8b color() const override {
        return pixel8b::from_normalized(1.0, 0.0, 0.0);
    }
};

#endif //RT_SPHERE_H
