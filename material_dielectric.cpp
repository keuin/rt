//
// Created by Keuin on 2022/4/15.
//

#include "material_dielectric.h"

bool material_dielectric::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_p = r.at(hit_t);
    assert(hit_obj.is_on(hit_p));
    auto n = hit_obj.normal_vector(hit_p);
    auto ri_ = ri_inv;
    if (dot(r.direction(), n) > 0) {
        // the ray is started from the object's inner,
        // use normal vector and ri on the surface's inner side
        n = -n;
        ri_ = 1.0 / ri_;
    }
    const auto r2 = n.refract<true>(r.direction(), ri_); // refracted vector
    r.direction(r2.unit_vec());
    r.source(hit_p);
    return true;
}
