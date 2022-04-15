//
// Created by Keuin on 2022/4/15.
//

#include "vec.h"
#include "material.h"
#include "material_reflective.h"

// perfectly-smooth reflective
template<>
bool material_reflective_<false>::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_point = r.at(hit_t);
    const auto nv = hit_obj.normal_vector(hit_point);
    const auto reflected = nv.reflect(r.direction());
    r.source(hit_point);
    r.direction(reflected);
    r.decay(albedo);
    return dot(reflected, nv) > 0;
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
    return dot(reflected, nv) > 0;
}