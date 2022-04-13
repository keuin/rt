//
// Created by Keuin on 2022/4/13.
//

#ifndef RT_AA_H
#define RT_AA_H

#include "vec.h"
#include "viewport.h"
#include <vector>
#include <thread>


// Antialiasing viewport
template<typename T>
class aa_viewport : public viewport<T> {
    unsigned samples;
    std::vector<basic_viewport<T>> subviews;

public:
    aa_viewport(double width, double height, vec3d viewport_center, unsigned samples) : samples(samples) {
        assert(samples >= 1);
        subviews = std::vector<basic_viewport<T>>{samples, {width, height, viewport_center}};
    }

    virtual bitmap<T> render(const hitlist<T> &world, vec3d viewpoint, uint16_t image_width, uint16_t image_height) {
//        // TODO threaded rendering
//        std::vector<std::thread> workers{samples};
//        for (size_t i = 0; i < samples; ++i) {
//            const auto &th = workers[i];
//            const auto &vw = subviews[i];
//        }
        const auto seed = 123456789012345678ULL;
        std::mt19937_64 seedgen{seed}; // generates seeds for workers
        std::vector<bitmap<T>> images{};
        for (const auto &view: subviews) {
            bias_ctx bc{seedgen()};
            images.push_back(view.render(world, viewpoint, image_width, image_height, bc));
        }
        return bitmap<T>::average(images);
    }

};

using aa_viewport8b = aa_viewport<uint8_t>;


#endif //RT_AA_H
