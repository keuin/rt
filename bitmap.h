//
// Created by Keuin on 2022/4/11.
//

#ifndef RT_BITMAP_H
#define RT_BITMAP_H

#include <vector>
#include <cstdint>
#include <ostream>
#include <cassert>
#include <vector>

#include "bitfont.h"
#include "vec.h"

#define COLORMIX_OVERFLOW_CHECK

//// T is some unsigned integer
//template<typename T>
//T saturate_add(T a, T b) {
//    T c = a + b;
//    if (a > c || b > c) {
//        c = (1ULL << (sizeof(T) * 8U)) - 1ULL;
//    }
//    return c;
//}

// T is some unsigned integer, do not use float point types!
template<typename T>
struct pixel {
    T r, g, b;
    static constexpr auto mod = (1ULL << (sizeof(T) * 8U)) - 1ULL; // FIXME

    bool operator==(const pixel<T> &other) const {
        return r == other.r && g == other.g && b == other.b;
    }

    /**
     * Create a pixel of given depth, from normalized color values.
     * r, g, b must in range [0, 1].
     * For example: Set color depth to 8bit, for normalized color (1, 0.5, 0.25), we get: (255, 127, 63).
     */
    static inline pixel<T> from_normalized(double r, double g, double b) {
        return pixel<T>{.r = (T) (mod * r), .g = (T) (mod * g), .b = (T) (mod * b)};
    }

    // v3d must be a normalized vector
    static inline pixel<T> from_normalized(const vec3d &v3d) {
        return from_normalized(v3d.x / 2.0 + 0.5, v3d.y / 2.0 + 0.5, v3d.z / 2.0 + 0.5);
    }

    // Convert pixels between different color depths.
    template<typename U>
    static inline pixel<T> from(const pixel<U> &orig) {
        return from_normalized(
                1.0 * orig.r / pixel<U>::max_value(),
                1.0 * orig.g / pixel<U>::max_value(),
                1.0 * orig.b / pixel<U>::max_value()
        );
    }

    static inline pixel<T> black() {
        return pixel<T>{(T) 0, (T) 0, (T) 0};
    }

    static inline pixel<T> white() {
        return pixel<T>{(T) mod, (T) mod, (T) mod}; // FIXME float-point values
    }

    static inline T max_value() {
        return mod; // FIXME
    }

    pixel<T> gamma2() const {
        const auto max = max_value();
        if (sizeof(T) <= 2) {
            // 26% faster than using double
            const auto r_ = sqrtf((float) (this->r) / (float) max);
            const auto g_ = sqrtf((float) (this->g) / (float) max);
            const auto b_ = sqrtf((float) (this->b) / (float) max);
            return pixel<T>::from_normalized(r_, g_, b_);
        } else {
            const auto r_ = sqrt(1.0 * this->r / max);
            const auto g_ = sqrt(1.0 * this->g / max);
            const auto b_ = sqrt(1.0 * this->b / max);
            return pixel<T>::from_normalized(r_, g_, b_);
        }
    }

};

//template<>
//pixel<uint16_t> pixel<uint16_t>::gamma2() const {
//    // 26% faster than using double
//    const auto max = max_value();
//    const auto r_ = sqrtf((float) (this->r) / (float) (max));
//    const auto g_ = sqrtf((float) (this->g) / (float) (max));
//    const auto b_ = sqrtf((float) (this->b) / (float) (max));
//    return pixel<uint16_t>::from_normalized(r_, g_, b_);
//}

template<
        typename T,
        typename S,
        typename = typename std::enable_if<std::is_arithmetic<S>::value, S>::type
>
pixel<T> operator*(const pixel<T> &pixel, S scale) {
    return ::pixel < T > {.r=(T) (pixel.r * scale), .g=(T) (pixel.g * scale), .b=(T) (pixel.b * scale)};
}

template<
        typename T,
        typename S,
        typename = typename std::enable_if<std::is_arithmetic<S>::value, S>::type
>
pixel<T> operator*(S scale, const pixel <T> &pixel) {
    return ::pixel < T > {.r=(T) (pixel.r * scale), .g=(T) (pixel.g * scale), .b=(T) (pixel.b * scale)};
}

// Mix two colors a and b. Returns a*u + b*v
template<typename T>
inline pixel<T> mix(const pixel<T> &a, const pixel<T> &b, double u, double v) {
    assert(u >= 0);
    assert(v >= 0);
    assert(u <= 1);
    assert(v <= 1);
    assert(u + v <= 1);
    pixel<T> c{0, 0, 0};
    c.r = (T) (u * a.r + v * b.r);
    c.g = (T) (u * a.g + v * b.g);
    c.b = (T) (u * a.b + v * b.b);
    return c;
}

// 8 bit pixel
using pixel8b = pixel<uint8_t>;

template<typename T>
class bitmap {
    unsigned width, height;
    std::vector<pixel<T>> content; // pixels scanned by rows, from top to bottom

    pixel<T> &image(unsigned x, unsigned y) {
        assert(x < width);
        assert(y < height);
        return content[x + y * width];
    }

    pixel<T> &image(unsigned x, unsigned y) const {
        assert(x < width);
        assert(y < height);
        return content[x + y * width];
    }

public:
    bitmap() = delete;

    bitmap(unsigned int width, unsigned int height) :
            width(width), height(height), content{width * height, pixel<T>{}} {}

    bitmap(unsigned int width, unsigned int height, std::vector<pixel<T>> &&data) :
            width(width), height(height), content{data} {}

    static bitmap<T> average(const std::vector<bitmap<T>> &images) {
        using Acc = typename std::conditional<
                (sizeof(T) <= 1),
                uint_fast16_t,
                typename std::conditional<
                        (sizeof(T) <= 2),
                        uint_fast32_t,
                        typename std::conditional<
                                (sizeof(T) <= 4),
                                uint_fast64_t,
                                uintmax_t
                        >::type
                >::type
        >::type; // pick the smallest suitable type for accumulator
        static_assert(sizeof(Acc) > sizeof(T), "accumulator may overflow");
        assert(!images.empty());
        bitmap<T> result{images[0].width, images[0].height};
        const auto m = images.size();
        const auto n = images[0].content.size();
        Acc acc_r, acc_g, acc_b;
        for (size_t i = 0; i < n; ++i) {
            acc_r = 0;
            acc_g = 0;
            acc_b = 0;
            for (size_t j = 0; j < m; ++j) {
                acc_r += images[j].content[i].r;
                acc_g += images[j].content[i].g;
                acc_b += images[j].content[i].b;
            }
            result.content[i] = pixel<T>{(T) (1.0 * acc_r / m), (T) (1.0 * acc_g / m), (T) (1.0 * acc_b / m)};
        }
        return result;
    }

    void set(unsigned x, unsigned y, const pixel<T> &pixel) {
        image(x, y) = pixel;
    }

    pixel<T> get(unsigned x, unsigned y) const {
        return image(x, y);
    }

    const pixel<T> &operator[](size_t loc) const {
        assert(loc < width * height);
        return content[loc];
    }

    pixel<T> &operator[](size_t loc) {
        assert(loc < width * height);
        return content[loc];
    }

    // Do not use float-point pixels, or this routine will break.
    void write_plain_ppm(std::ostream &out) const;

    // Draw text on the image. Supports limited visual ASCII characters.
    void print(const std::string &s, const pixel<T> &color,
               unsigned x, unsigned y, unsigned scale = 1, double alpha = 1.0);

    bool normalized() const {
        return false;
        // TODO return true for float-point pixels
    }

    std::pair<unsigned, unsigned> shape() const {
        return std::pair<unsigned, unsigned>{width, height};
    }

    // U: original color depth, T: desired color depth
    template<typename U>
    static bitmap<T> from(const bitmap<U> &src) {
        const auto shape = src.shape();
        const size_t sz = shape.first * shape.second;
        bitmap<T> out{shape.first, shape.second};
        for (size_t i = 0; i < sz; ++i) {
            out[i] = pixel<T>::from(src[i]);
        }
        return out;
    }

    bitmap<T> gamma2() const {
        std::vector<pixel<T>> out;
        out.reserve(content.size());
        for (const auto &pix: content) {
            out.push_back(pix.gamma2());
        }
        return bitmap<T>{width, height, std::move(out)};
    }

};

template<typename T>
void bitmap<T>::write_plain_ppm(std::ostream &out) const {
    const auto depth = (1ULL << (sizeof(T) * 8)) - 1ULL; // color depth of pixels
    out << "P3\n" // file header
        << width << ' ' << height << '\n' // image width and height
        << depth << '\n'; // normalized max color depth (e.g. 255 for 8bit image)

    for (const auto &pixel: content) {
        out << (unsigned long long) pixel.r << ' '
            << (unsigned long long) pixel.g << ' '
            << (unsigned long long) pixel.b << '\n';
    }
}

template<typename T>
void bitmap<T>::print(const std::string &s, const pixel<T> &color,
                      unsigned x, unsigned y, unsigned scale, double alpha) {
    assert(alpha >= 0);
    assert(alpha <= 1);
    const unsigned char_w = 8 * scale, char_h = 13 * scale; // char width and height
    size_t n = 0;
    for (const auto &c: s) {
        unsigned int idx = c - 32U;
        if (idx >= 95) idx = 1; // replace invisible chars with '!'

        const unsigned char_x = x + n * char_w + ((n > 1) ? ((n - 1) * scale) : 0);
        const unsigned char_y = y;

        // char size is 13x8, stored line by line, from the bottom line to the top line
        // every line is represented in a single byte
        // in every byte, from MSB to LSB, the pixel goes from left to right

        for (unsigned i = 0; i < char_w; ++i) {
            for (unsigned j = 0; j < char_h; ++j) {
                // downscale (i, j) to pos on char bitmap i -> i/scale, j -> j/scale
                if (rasters[idx][12 - j / scale] & (1U << (7U - i / scale))) {
                    const auto pen_x = char_x + i, pen_y = char_y + j;
                    if (pen_x >= width || pen_y >= height) continue;
                    image(pen_x, pen_y) = mix(image(pen_x, pen_y), color, 1.0 - alpha, alpha);
                }
            }
        }
        ++n;
    }
}

using bitmap8b = bitmap<uint8_t>;


#endif //RT_BITMAP_H
