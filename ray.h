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

public:
    ~ray3() = default;

    ray3() = delete;

    ray3(const ray3<T> &other) = default;

    ray3<T> &operator=(const ray3<T> &other) {
        source_ = other.source_;
        direction_ = other.direction_;
        return *this;
    }

    ray3(const vec3<T> &source, const vec3<T> &direction) : source_(source), direction_(direction.unit_vec()) {}

    // Get the source point from where the ray emits.
    vec3<T> source() const {
        return source_;
    }

    // Get the unit vector along the ray's direction.
    vec3<T> direction() const {
        return direction_;
    }

    // Compute the point this ray reaches at the time `t`.
    template<typename U>
    vec3<T> at(U t) const {
        return source_ + direction_ * t;
    }

    // Get a ray starts from zero and directs to undefined direction.
    static ray3<T> null() {
        return ray3<T>{vec3<T>::zero(), vec3<T>::zero()};
    }
};

using ray3d = ray3<double>;

#endif //RT_RAY_H
