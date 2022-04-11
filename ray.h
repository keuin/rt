//
// Created by Keuin on 2022/4/11.
//

#ifndef RT_RAY_H
#define RT_RAY_H

#include "vec.h"

// A ray is a half-line, starts from a 3d point, along the direction of a unit vector, to the infinity
template<typename T>
class ray3 {
    const vec3<T> source_;
    const vec3<T> direction_; // unit vector

public:
    ray3() = delete;
    ray3(const vec3<T> &source, const vec3<T> &direction) : source_(source), direction_(direction.unit_vec()) {}

    vec3<T> source() const {
        return source_;
    }

    vec3<T> direction() const {
        return direction_;
    }

    template<typename U>
    vec3<T> at(U t) const {
        return source_ + direction_ * t;
    }
};

using ray3d = ray3<double>;

#endif //RT_RAY_H
