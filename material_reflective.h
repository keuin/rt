//
// Created by Keuin on 2022/4/15.
//

#ifndef RT_MATERIAL_REFLECTIVE_H
#define RT_MATERIAL_REFLECTIVE_H

#include "vec.h"
#include "material.h"

// metal
class material_reflective : public material {
    vec3d albedo;
public:
    explicit material_reflective(vec3d &color) : albedo(color) {}

    bool scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const override;
};

#endif //RT_MATERIAL_REFLECTIVE_H
