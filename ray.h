//
// Created by Keuin on 2022/4/11.
//

#ifndef RT_RAY_H
#define RT_RAY_H

#include "vec.h"

// A ray is a half-line, starts from a 3d point, along the direction of a unit vector, to the infinity
template<typename T>
class ray3 {
    vec3<T> source_;
    vec3<T> direction_; // unit vector
    double decay_; // How much power remaining

public:
    ~ray3() = default;

    ray3() = delete;

    ray3(const ray3<T> &other) = default;

    ray3<T> &operator=(const ray3<T> &other) {
        source_ = other.source_;
        direction_ = other.direction_;
        decay_ = other.decay_;
        return *this;
    }

    ray3(const vec3<T> &source, const vec3<T> &direction) :
            source_(source), direction_(direction.unit_vec()), decay_(1.0) {}

    // Get the source point from where the ray emits.
    vec3<T> source() const {
        return source_;
    }

    void source(const vec3<T> &s) {
        source_ = s;
    }

    // Get the unit vector along the ray's direction.
    vec3<T> direction() const {
        return direction_;
    }

    // d should be a unit vector
    void direction(const vec3<T> &d) {
        assert(fabs(d.mod2() - 1.0) < 1e-6);
        direction_ = d;
    }

    // Compute the point this ray reaches at the time `t`.
    template<typename U>
    vec3<T> at(U t) const {
        return source_ + direction_ * t;
    }

    // Get the final color we got at the ray's origin, by tracing back along with the ray's path.
    // Currently, here is only a simple power decay accumulation.
    template<typename U>
    pixel<U> hit(const pixel<U> &color) const {
        return decay_ * color;
    }

    void decay(double a) {
        decay_ *= a;
    }

    double decay() const {
        return decay_;
    }

    // Get a ray starts from zero and directs to undefined direction.
    static ray3<T> null() {
        return ray3<T>{vec3<T>::zero(), vec3<T>::zero()};
    }
};

using ray3d = ray3<double>;

#endif //RT_RAY_H
