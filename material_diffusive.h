//
// Created by Keuin on 2022/4/15.
//

#ifndef RT_MATERIAL_DIFFUSIVE_H
#define RT_MATERIAL_DIFFUSIVE_H

#include "vec.h"
#include "material.h"

class material_diffuse_lambertian : public material {
    vec3d albedo;
public:
    explicit material_diffuse_lambertian(vec3d albedo);
    explicit material_diffuse_lambertian(double albedo);

    bool scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const override;
};

class material_diffuse_simple : public material {
    vec3d albedo;
public:
    explicit material_diffuse_simple(vec3d albedo);
    explicit material_diffuse_simple(double albedo);

    bool scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const override;
};

class material_diffuse_hemispherical : public material {
    vec3d albedo;
public:
    explicit material_diffuse_hemispherical(vec3d albedo);
    explicit material_diffuse_hemispherical(double albedo);

    bool scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const override;
};

#endif //RT_MATERIAL_DIFFUSIVE_H
