//
// Created by Keuin on 2022/4/15.
//

#include "material_dielectric.h"
#include "tracelog.h"

bool material_dielectric::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_p = r.at(hit_t);
    assert(hit_obj.is_on(hit_p));
    auto nv = hit_obj.normal_vector(hit_p);
    const auto n1 = r.direction(); // incoming ray, unit vector
    const bool outside = dot(nv, n1) < 0; // if the ray hits the outside of the surface
    double rel_ri; // relative ri, source_ri/target_ri
    if (!outside) {
        nv = -nv; // use normal vector pointing to the inner
        rel_ri = ri;
    } else {
        rel_ri = 1.0 / ri;
    }
    const auto cos_t1 = std::min(-dot(nv, n1), 1.0); // cos(theta1)
    const auto sin_t1_s = 1 - cos_t1 * cos_t1; // sin(theta1)^2
    const auto rel_ri_s = rel_ri * rel_ri;
    const auto sin_t2_s = sin_t1_s * rel_ri_s;
    double p_reflect; // probability of schlick reflection
    bool reflect = false;
    if (sin_t2_s > 1.0) {
        TRACELOG("    reflect (TIR, ri=%f)\n", ri);
        // sin(theta2)^2 > 1, no real solution
        // TIR (total internal reflection)
        reflect = true;
    } else if ((p_reflect = reflectance(cos_t1, ri)) > ruvg.range01_scalar()) {
        TRACELOG("    reflect (schlick, ri=%f, p=%f)\n", ri, p_reflect);
        reflect = true;
    }
    if (reflect) {
        r.direction(nv.reflect(n1));
        r.source(hit_p);
        return true;
    } else {
        // normal refract
        TRACELOG("    refract (forced, ri=%f)\n", ri);
        const auto sin_t2 = sqrt(sin_t2_s);
        const auto cos_t2 = sqrt(1 - sin_t2_s);
        const auto n2 = (-cos_t2 * nv) + (n1 + cos_t1 * nv).unit_vec() * sin_t2;
        r.direction(n2);
        r.source(hit_p);
        return true;
    }
}
