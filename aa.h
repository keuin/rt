//
// Created by Keuin on 2022/4/13.
//

#ifndef RT_AA_H
#define RT_AA_H

#include "vec.h"
#include "viewport.h"
#include <vector>
#include <thread>
#include <algorithm>

// Antialiasing viewport
template<typename T>
class aa_viewport : public viewport<T> {
    unsigned samples;
    std::vector<basic_viewport<T>> *subviews;

public:
    aa_viewport(double width, double height, vec3d viewport_center, unsigned samples) : samples(samples) {
        assert(samples >= 1);
        subviews = new std::vector<basic_viewport<T>>{samples, {width, height, viewport_center}};
    }

    ~aa_viewport() {
        delete subviews;
    }

    virtual bitmap<T> render(const hitlist<T> &world, vec3d viewpoint, uint16_t image_width, uint16_t image_height) {
        const unsigned hwcc = std::thread::hardware_concurrency();
        std::cerr << "Rendering with " << hwcc << " thread(s)." << std::endl;

        const auto seed = 123456789012345678ULL;
        std::mt19937_64 seedgen{seed}; // generates seeds for workers
        std::vector<bitmap<T>> images{samples, {1, 1}};
        std::thread t;

        std::vector<std::thread> workers;
        unsigned remaining = samples; // tasks remaining
        size_t base = 0;
        while (remaining > 0) {
            const unsigned n = std::min(hwcc, remaining); // threads in current batch
            remaining -= n;
            for (unsigned i = 0; i < n; ++i) {
                workers.emplace_back(std::thread{
                        [&](int tid, uint64_t seed, std::vector<basic_viewport<T>> *subs, vec3d viewpoint,
                            uint16_t image_width, uint16_t image_height) {
                            bias_ctx bc{seed};
                            auto image = (*subs)[tid].render(world, viewpoint, image_width, image_height, bc);
                            images[base + tid] = image;
                        },
                        i, seedgen(), subviews, viewpoint, image_width, image_height
                });
            }
            for (auto &th: workers) {
                th.join();
            }
            workers.clear();
            base += n;
        }
        return bitmap<T>::average(images);
    }

};

using aa_viewport8b = aa_viewport<uint8_t>;


#endif //RT_AA_H
