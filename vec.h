//
// Created by Keuin on 2022/4/11.
//

#ifndef RT_VEC_H
#define RT_VEC_H

#include <cmath>
#include <random>
#include <ostream>
#include <cassert>
#include <algorithm>

static inline bool eq(int a, int b) {
    return a == b;
}

static inline bool eq(long long a, long long b) {
    return a == b;
}

static inline bool eq(double a, double b) {
    // FIXME broken on large values
    // https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
    // https://stackoverflow.com/a/253874/14332799
    const double c = a - b;
    return c <= 1e-14 && c >= -1e-14;
}

// 3-dim vector
template<typename T>
struct vec3 {
    T x;
    T y;
    T z;

    static vec3 zero() {
        return vec3{0, 0, 0};
    }

    static vec3 one() {
        return vec3{1, 1, 1};
    }

    bool is_zero() const {
#define EPS (1e-8)
        return x >= -EPS && y >= -EPS && z >= -EPS && x <= EPS && y <= EPS && z <= EPS;
#undef EPS
    }

    bool is_one() const {
        return (*this - vec3::one()).is_zero();
    }

    vec3 operator+(const vec3 &b) const {
        return vec3{.x=x + b.x, .y=y + b.y, .z=z + b.z};
    }

    vec3 operator-() const {
        return vec3{.x = -x, .y = -y, .z = -z};
    }

    vec3 operator-(const vec3 &b) const {
        return *this + (-b);
    }

    bool operator==(const vec3 b) const {
        return eq(x, b.x) && eq(y, b.y) && eq(z, b.z);
    }

    // dot product (aka inner product, or scalar product, producing a scalar)
    T dot(const vec3 &b) const {
        return x * b.x + y * b.y + z * b.z;
    }

    // cross product (aka outer product, or vector product, producing a vector)
    vec3 cross(const vec3 &b) const {
        return vec3{.x=y * b.z - z * b.y, .y=z * b.x - x * b.z, .z=x * b.y - y * b.x};
    }

    // Multiply with b on every dimension.
    vec3 scale(const vec3 &b) const {
        return vec3{.x=x * b.x, .y=y * b.y, .z=z * b.z};
    }

    // norm value
    double norm(const int level = 2) const {
        if (level == 2) {
            return std::abs(sqrt(x * x + y * y + z * z));
        } else if (level == 1) {
            return std::abs(x) + std::abs(y) + std::abs(z);
        } else {
            return powl(powl(x, level) + powl(y, level) + powl(z, level), 1.0 / level);
        }
    }

    // Squared module
    T mod2() const {
        return x * x + y * y + z * z;
    }

    vec3 unit_vec() const {
        return *this * (1.0 / norm());
    }

    // If all the three float-point scalars are finite real number.
    bool valid() const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }

    // Determine if this vector is parallel with another one.
    bool parallel(const vec3 &other) const {
        const auto dt = dot(other);
        const auto dot2 = dt * dt;
        const auto sqlp = mod2() * other.mod2(); // squared length product
        const auto d = dot2 - sqlp;
        return d > -1e-6 && d < 1e-6;
    }

    // Get the reflected vector. Current vector is the normal vector (length should be 1), v is the incoming vector.
    vec3 reflect(const vec3 &v) const {
        assert(fabs(mod2() - 1.0) < 1e-8);
        return v - (2.0 * dot(v)) * (*this);
    }
};

// print to ostream
template<typename T>
inline std::ostream &operator<<(std::ostream &out, const vec3<T> &vec) {
    return out << "vec3[x=" << vec.x << ", y=" << vec.y << ", z=" << vec.z << ']';
}

// product vec3 by a scalar, with fp assertions
template<
        typename T,
        typename S,
        typename = typename std::enable_if<std::is_arithmetic<S>::value, S>::type
>
inline vec3<T> operator*(const vec3<T> &vec, const S &b) {
    if (std::is_floating_point<S>::value) {
        assert(std::isfinite(b));;
    }
    return vec3<T>{.x=(T) (vec.x * b), .y=(T) (vec.y * b), .z=(T) (vec.z * b)};
}

// product vec3 by a scalar, with fp assertions
template<
        typename T,
        typename S
>
inline vec3<T> operator*(const S &b, const vec3<T> &vec) {
    if (std::is_floating_point<S>::value) {
        assert(std::isfinite(b));
    }
    return vec3<T>{.x=(T) (vec.x * b), .y=(T) (vec.y * b), .z=(T) (vec.z * b)};
}

// product vec3 by the inversion of a scalar (div by a scalar), with fp assertions
template<
        typename T,
        typename S,
        typename = typename std::enable_if<std::is_arithmetic<S>::value, S>::type
>
inline vec3<T> operator/(const vec3<T> &vec, const S &b) {
    if (std::is_floating_point<S>::value) {
        assert(std::isfinite(b));
        assert(b != 0);
    }
    return vec3<T>{.x=(T) (vec.x / b), .y=(T) (vec.y / b), .z=(T) (vec.z / b)};
}

// scalar product (inner product)
template<typename T>
inline T dot(const vec3<T> &a, const vec3<T> &b) {
    return a.dot(b);
}

// vector product (outer product)
template<typename T>
inline vec3<T> cross(const vec3<T> &a, const vec3<T> &b) {
    return a.cross(b);
}

// 3-dim vector (int)
using vec3i = vec3<int>;

// 3-dim vector (long long)
using vec3l = vec3<long long>;

// 3-dim vector (float)
using vec3f = vec3<float>;

// 3-dim vector (double)
using vec3d = vec3<double>;

// random unit vector generator
template<typename T>
class rand_vec_gen {
    std::mt19937_64 mt;
    std::uniform_real_distribution<T> uni{-1.0, 1.0};

public:
    rand_vec_gen() = delete;

    explicit rand_vec_gen(uint64_t seed) : mt{seed} {}

    // Get a random vector whose length is in [0, 1]
    inline vec3<T> range01() {
        while (true) {
            const auto x = uni(mt), y = uni(mt), z = uni(mt);
            const auto vec = vec3<T>{.x=x, .y=y, .z=z};
            if (vec.mod2() <= 1.0) return vec.unit_vec();
        }
    }

    // Get a random real number in range [0, 1].
    inline T range01_scalar() {
        return uni(mt);
    };

    // Get a unit vector with random direction.
    inline vec3<T> normalized() {
        return range01().unit_vec();
    }

    // Get a random vector whose length is in [0, 1] and
    // has a direction difference less than 90 degree with given vector.
    inline vec3<T> hemisphere(vec3<T> &vec) {
        const auto v = range01();
        if (dot(v, vec) > 0) return v;
        return -v;
    }
};

using random_uv_gen_3d = rand_vec_gen<double>;

#endif //RT_VEC_H