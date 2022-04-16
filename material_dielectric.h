//
// Created by Keuin on 2022/4/15.
//

#ifndef RT_MATERIAL_DIELECTRIC_H
#define RT_MATERIAL_DIELECTRIC_H

#include "vec.h"
#include "material.h"

class material_dielectric : public material {
    double ri_inv;
public:
    explicit material_dielectric(double ri) : ri_inv{1.0 / ri} {}
    bool scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const override;
};

#endif //RT_MATERIAL_DIELECTRIC_H
