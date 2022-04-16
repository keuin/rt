//
// Created by Keuin on 2022/4/15.
//

#include "material_dielectric.h"

bool material_dielectric::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_p = r.at(hit_t);
    assert(hit_obj.is_on(hit_p));
    auto n = hit_obj.normal_vector(hit_p);
    auto ri_ = ri_inv;
    auto cos1 = dot(r.direction(), n); // -cos(a1)
    if (cos1 > 0) {
        // the ray is started from the object's inner,
        // use normal vector and ri on the surface's inner side
        n = -n;
        ri_ = 1.0 / ri_;
    } else {
        cos1 = -cos1;
    }
    vec3d r2;
    // determine reflection or refraction using Schlick's Approximation.
    if (reflectance(cos1, ri_) > ruvg.range01_scalar()) {
        // reflect
        r2 = n.reflect(r.direction());
    } else {
        // refract
        r2 = n.refract<true>(r.direction(), ri_);
    }
    r.direction(r2.unit_vec());
    r.source(hit_p);
    return true;
}
