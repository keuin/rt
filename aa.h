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
    int threads;

public:
    aa_viewport(double width, double height, vec3d viewport_center, unsigned samples, int threads = -1)
            : samples(samples), threads{(threads > 0) ? threads : (int)std::thread::hardware_concurrency()} {
        assert(samples >= 1);
        subviews = new std::vector<basic_viewport<T>>{samples, {width, height, viewport_center}};
    }

    ~aa_viewport() {
        delete subviews;
    }

    virtual bitmap<T> render(const hitlist &world, vec3d viewpoint, uint16_t image_width, uint16_t image_height) {
        static constexpr auto seed = 123456789012345678ULL;
        const unsigned thread_count = std::min((unsigned)threads, samples);
        std::cerr << "Preparing tasks..." << std::endl;
        std::vector<bitmap<T>> images{samples, {0, 0}};
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
        timer tim{true};

        std::cerr << "Seeding tasks..." << std::endl;
        tim.start_measure();
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
        tim.stop_measure();
        std::cerr << "Finish seeding. Speed: " << 1.0 * samples / tim.duration().count() << " task/sec" << std::endl;

        std::cerr << "Rendering with " << thread_count << " thread(s)." << std::endl;
        tim.start_measure();
        pool.start();
        pool.wait();
        tim.stop_measure();
        std::cerr << "Finish rendering sub-pixels. Speed: " << 1.0 * tim.duration().count() / samples << "sec/image, "
                  << 1.0 * samples * image_width * image_height / tim.duration().count() << " pixel/sec" << std::endl;
        return bitmap<T>::average(images);
    }

};

using aa_viewport8b = aa_viewport<uint8_t>;


#endif //RT_AA_H
