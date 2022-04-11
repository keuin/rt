//
// Created by Keuin on 2022/4/11.
//

#include <cmath>

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

    vec3 operator+(vec3 &b) const {
        return vec3{.x=x + b.x, .y=y + b.y, .z=z + b.z};
    }

    vec3 operator-() const {
        return vec3{.x = -x, .y = -y, .z = -z};
    }

    vec3 operator-(vec3 &b) const {
        return vec3{.x=x - b.x, .y=y - b.y, .z=z - b.z};
    }

    // cross product
    vec3 operator*(vec3 &b) const {
        return vec3{.x=y * b.z - z * b.y, .y=x * b.z - z * b.x, .z=x * b.y - y * b.x};
    }

    bool operator==(vec3 b) const {
        return eq(x, b.x) && eq(y, b.y) && eq(z, b.z);
    }

    // dot product
    int dot(vec3 &b) const {
        return x * b.x + y * b.y + z * b.z;
    }

    // norm value
    int norm(int level = 2) const {
        if (level == 2) {
            return std::abs(x * x + y * y + z * z);
        } else if (level == 1) {
            return std::abs(x + y + z);
        } else {
            return (int) std::abs(powl(x, level) + powl(y, level) + powl(z, level));
        }
    }
};

// 3-dim vector (int)
using vec3i = vec3<int>;

// 3-dim vector (long long)
using vec3l = vec3<long long>;

// 3-dim vector (float)
using vec3f = vec3<float>;

// 3-dim vector (double)
using vec3d = vec3<double>;
