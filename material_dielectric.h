//
// Created by Keuin on 2022/4/15.
//

#ifndef RT_MATERIAL_DIELECTRIC_H
#define RT_MATERIAL_DIELECTRIC_H

#include "vec.h"
#include "material.h"

class material_dielectric : public material {
    double ri_inv;

    static double reflectance(double cosine, double ref_idx) {
        assert(cosine > 0);
        assert(ref_idx > 0);
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

public:
    explicit material_dielectric(double ri) : ri_inv{1.0 / ri} {}

    bool scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const override;
};

#endif //RT_MATERIAL_DIELECTRIC_H
