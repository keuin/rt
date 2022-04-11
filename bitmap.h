//
// Created by Keuin on 2022/4/11.
//

#ifndef RT_BITMAP_H
#define RT_BITMAP_H

#include <vector>
#include <cstdint>
#include <ostream>
#include <cassert>


// T is some unsigned integer, do not use float point types!
template<typename T>
struct pixel {
    T r, g, b;
};

// 8 bit pixel
using pixel8b = pixel<uint8_t>;

template<typename T>
class bitmap {
    const unsigned width, height;
    std::vector<pixel<T>> content; // pixels scanned by rows, from top to bottom

    pixel<T> &image(unsigned x, unsigned y) {
        assert(x < width);
        assert(y < height);
        return content[x * width + y];
    }

    pixel<T> &image(unsigned x, unsigned y) const {
        assert(x < width);
        assert(y < height);
        return content[x * width + y];
    }

public:
    bitmap(unsigned int width, unsigned int height) : width(width), height(height) {
        content.resize(width * height, pixel<T>{});
    }

    void set(unsigned x, unsigned y, const pixel<T> &pixel) {
        image(x, y) = pixel;
    }

    pixel<T> get(unsigned x, unsigned y) const {
        return image(x, y);
    }

    // Do not use float-point pixels, or this routine will break.
    void write_plain_ppm(std::ostream &out) const;
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

using bitmap8b = bitmap<uint8_t>;


#endif //RT_BITMAP_H
