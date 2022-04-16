//
// Created by Keuin on 2022/4/13.
//

#ifndef RT_AA_H
#define RT_AA_H

#include "vec.h"
#include "viewport.h"
#include "hitlist.h"
#include "threading.h"
#include <vector>
#include <algorithm>
#include <random>

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

    virtual bitmap<T> render(const hitlist &world, vec3d viewpoint, uint16_t image_width, uint16_t image_height) {
        static constexpr auto seed = 123456789012345678ULL;
        const unsigned thread_count = std::min(std::thread::hardware_concurrency(), samples);
        std::cerr << "Preparing tasks..." << std::endl;
        std::vector<bitmap<T>> images{samples, {1, 1}};
        std::mt19937_64 seedgen{seed}; // generates seeds for workers

        const struct s_render_shared {
            std::vector<basic_viewport<T>> &subs;
            vec3d viewpoint;
            uint16_t image_width;
            uint16_t image_height;
            const hitlist &world;
            std::vector<bitmap<T>> &images;
        } s_{.subs=*subviews, .viewpoint = viewpoint,
                .image_width=image_width, .image_height=image_height,
                .world=world, .images=images
        };

        struct s_render_task {
            uint32_t task_id;
            uint64_t seed;
            uint64_t diffuse_seed;
            const s_render_shared &shared;
        };

        thread_pool<s_render_task> pool{thread_count};


        for (typeof(samples) i = 0; i < samples; ++i) {
            pool.submit_task([](s_render_task &task) {
                bias_ctx bc{seed};
                auto image = task.shared.subs[task.task_id].render(
                        task.shared.world, task.shared.viewpoint,
                        task.shared.image_width, task.shared.image_height,
                        bc, task.diffuse_seed);
                task.shared.images[task.task_id] = image;
            }, s_render_task{
                    .task_id = i, .seed=seedgen(), .diffuse_seed=seedgen(), .shared=s_
            });
        }

        std::cerr << "Rendering with " << thread_count << " thread(s)." << std::endl;
        pool.start();
        pool.wait();
        return bitmap<T>::average(images);
    }

};

using aa_viewport8b = aa_viewport<uint8_t>;


#endif //RT_AA_H
