//
// Created by Keuin on 2022/4/15.
//

#include "vec.h"
#include "material.h"
#include "material_reflective.h"

bool material_reflective::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_point = r.at(hit_t);
    const auto nv = hit_obj.normal_vector(hit_point);
    const auto reflected = nv.reflect(r.direction());
    r.source(hit_point);
    r.direction(reflected);
    r.decay(albedo);
    return dot(reflected, nv) > 0;
}
