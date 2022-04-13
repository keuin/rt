//
// Created by Keuin on 2022/4/11.
//

#ifndef RT_VEC_H
#define RT_VEC_H

#include <cmath>
#include <random>

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
        return vec3{.x=y * b.z - z * b.y, .y=x * b.z - z * b.x, .z=x * b.y - y * b.x};
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
};

// print to ostream
template<typename T>
inline std::ostream &operator<<(std::ostream &out, const vec3<T> &vec) {
    return out << "vec3[x=" << vec.x << ", y=" << vec.y << ", z=" << vec.z << ']';
}

// product vec3 by a scalar
template<
        typename T,
        typename S,
        typename = typename std::enable_if<std::is_arithmetic<S>::value, S>::type
>
inline vec3<T> operator*(const vec3<T> &vec, const S &b) {
    return vec3<T>{.x=(T) (vec.x * b), .y=(T) (vec.y * b), .z=(T) (vec.z * b)};
}

// product vec3 by a scalar
template<
        typename T,
        typename S,
        typename = typename std::enable_if<std::is_arithmetic<S>::value, S>::type
>
inline vec3<T> operator*(const S &b, const vec3<T> &vec) {
    return vec3<T>{.x=(T) (vec.x * b), .y=(T) (vec.y * b), .z=(T) (vec.z * b)};
}

// product vec3 by the inversion of a scalar (div by a scalar)
template<
        typename T,
        typename S,
        typename = typename std::enable_if<std::is_arithmetic<S>::value, S>::type
>
inline vec3<T> operator/(const vec3<T> &vec, const S &b) {
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
class random_uv_gen {
    std::mt19937_64 mt;
    std::uniform_real_distribution<T> uni{-1.0, 1.0};

public:
    explicit random_uv_gen(uint64_t seed) : mt{seed} {}

    vec3<T> operator()() {
        while (true) {
            const auto x = uni(mt), y = uni(mt), z = uni(mt);
            const auto vec = vec3<T>{.x=x, .y=y, .z=z};
            if (vec.mod2() <= 1.0) return vec;
        }
    }
};

using random_uv_gen_3d = random_uv_gen<double>;

#endif //RT_VEC_H