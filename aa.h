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
// U: color depth, V: pos
template<typename U, typename V>
class aa_viewport {
    vec3<V> cxyz;
    vec3<V> screen_center;
    uint32_t image_width; // how many pixels every row has
    uint32_t image_height; // how many pixels every column has
    V screen_hw; // determined if screen_height is known
    V screen_hh; // determined if screen_width is known
    hitlist &world;
    unsigned samples;
    int threads;
    double aperture;
    double focus_dist;

public:

    aa_viewport(const vec3<V> &cxyz,
                const vec3<V> &screen_center,
                uint16_t image_width,
                uint16_t image_height,
                V screen_hw,
                V screen_hh,
                double aperture,
                double focus_dist,
                hitlist &world,
                unsigned samples,
                int threads = -1) :
            cxyz(cxyz),
            screen_center(screen_center),
            image_width(image_width),
            image_height(image_height),
            screen_hw(screen_hw),
            screen_hh(screen_hh),
            world(world),
            samples(samples),
            threads((threads > 0) ? threads : (int) std::thread::hardware_concurrency()),
            aperture{aperture},
            focus_dist{focus_dist} {
        assert(samples >= 1);
        assert(std::abs(1.0 * image_width / image_height - 1.0 * screen_hw / screen_hh) < 1e-8);
    }

    aa_viewport(const vec3<V> &cxyz,
                const vec3<V> &screen_center,
                uint16_t image_width,
                uint16_t image_height,
                double fov_h,
                double aperture,
                double focus_dist,
                hitlist &world,
                unsigned samples,
                int threads = -1) :
            cxyz(cxyz),
            screen_center(screen_center),
            image_width(image_width),
            image_height(image_height),
            screen_hw{(cxyz - screen_center).norm() * tan((double) fov_h / 2.0)},
            screen_hh{screen_hw * ((double) image_height / image_width)},
            world(world),
            samples(samples),
            threads((threads > 0) ? threads : (int) std::thread::hardware_concurrency()),
            aperture{aperture},
            focus_dist{focus_dist} {
        assert(samples >= 1);
    }

    bitmap<U> render() {
        static constexpr auto seed = 123456789012345678ULL;
        const unsigned thread_count = std::min((unsigned) threads, samples);
        std::cerr << "Preparing tasks..." << std::endl;
        std::vector<bitmap<U>> images{samples, {0, 0}};
        std::mt19937_64 seedgen{seed}; // generates seeds for workers

        struct s_render_task {
            uint64_t bias_seed;
            uint64_t diffuse_seed;
        };

        thread_pool<s_render_task, decltype(*this), decltype(images)> pool{thread_count, *this, images, samples};
        timer tim{true};

        std::cerr << "Seeding tasks..." << std::endl;
        tim.start_measure();
        for (decltype(samples) i = 0; i < samples; ++i) {
            pool.submit_task([](size_t tid, s_render_task &task, const aa_viewport<U, V> &ctx, decltype(images) &images) {
                basic_viewport<U, V> vp{
                        ctx.cxyz, ctx.screen_center,
                        ctx.image_width, ctx.image_height,
                        ctx.screen_hw, ctx.screen_hh,
                        ctx.aperture, ctx.focus_dist, ctx.world
                };
                bias_ctx bc{task.bias_seed};
                bokeh_ctx bokeh{task.diffuse_seed + 6543210987ULL};
                images[tid] = vp.render(task.diffuse_seed, bc, bokeh);
            }, s_render_task{
                    seedgen(), seedgen()
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
        return bitmap<U>::average(images);
    }

};


#endif //RT_AA_H
