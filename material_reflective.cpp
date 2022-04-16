//
// Created by Keuin on 2022/4/15.
//

#include "vec.h"
#include "material.h"
#include "material_reflective.h"
#include "tracelog.h"

// perfectly-smooth reflective
template<>
bool material_reflective_<false>::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_point = r.at(hit_t);
    const auto nv = hit_obj.normal_vector(hit_point);
    const auto reflected = nv.reflect(r.direction());
    r.source(hit_point);
    r.direction(reflected);
    r.decay(albedo);
    const auto alive = dot(reflected, nv) > 0;
#ifdef LOG_TRACE
    if (!alive) {
        TRACELOG("    absorb (perfectly smooth material) (reflected: [%-10f,%-10f,%-10f], nv: [%-10f,%-10f,%-10f])\n",
                 reflected.x, reflected.y, reflected.z, nv.x, nv.y, nv.z);
    } else {
        TRACELOG("    reflect (perfectly smooth material)\n");
    }
#endif
    return alive;
}

// fuzzy reflective
template<>
bool material_reflective_<true>::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_point = r.at(hit_t);
    const auto nv = hit_obj.normal_vector(hit_point);
    const auto reflected = nv.reflect(r.direction()) + fuzzy_.f * ruvg.range01();
    r.source(hit_point);
    r.direction(reflected.unit_vec());
    r.decay(albedo);
    const auto alive = dot(reflected, nv) > 0;
#ifdef LOG_TRACE
    if (!alive) {
        TRACELOG("    absorb (fuzzy reflective material) (reflected: [%-10f,%-10f,%-10f], nv: [%-10f,%-10f,%-10f])\n",
                 reflected.x, reflected.y, reflected.z, nv.x, nv.y, nv.z);
    } else {
        TRACELOG("    reflect (fuzzy reflective material)\n");
    }
#endif
    return alive;
}