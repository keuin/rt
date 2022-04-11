//
// Created by Keuin on 2022/4/11.
//

#include <iostream>
#include "bitmap.h"

// write a test .PPM image
int main() {
    bitmap8b image{256, 256};
    pixel8b p{};
    for (int x = 0; x < 256; ++x) {
        for (int y = 0; y < 256; ++y) {
            p.r = x;
            p.b = y;
            image.set(x, y, p);
        }
    }
    image.write_plain_ppm(std::cout);
}