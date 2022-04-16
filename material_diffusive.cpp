//
// Created by Keuin on 2022/4/15.
//

#include "vec.h"
#include "material_diffusive.h"
#include "tracelog.h"


bool material_diffuse_lambertian::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_point = r.at(hit_t); // hit point, on the surface
    auto nv = hit_obj.normal_vector(hit_point);
    if (dot(nv, r.direction()) > 0) return false; // discard rays from inner (or invert nv)
    auto d = nv + ruvg.normalized();
    if (d.is_zero()) d = nv;
    vec3d diffuse_target = hit_point + d;
    r.decay(albedo); // lose some light when diffused
    r.source(hit_point);
    r.direction((diffuse_target - hit_point).unit_vec()); // the new diffused ray we trace on
    TRACELOG("    reflected (diffusive, lambertian material)\n");
    return true;
}

material_diffuse_lambertian::material_diffuse_lambertian(vec3d albedo) : albedo(albedo) {
    assert(albedo.x >= 0);
    assert(albedo.y >= 0);
    assert(albedo.z >= 0);
    assert(albedo.x <= 1);
    assert(albedo.y <= 1);
    assert(albedo.z <= 1);
}

material_diffuse_lambertian::material_diffuse_lambertian(double albedo) : albedo{albedo, albedo, albedo} {
    assert(albedo >= 0);
    assert(albedo <= 1);
}

material_diffuse_simple::material_diffuse_simple(vec3d albedo) : albedo(albedo) {
    assert(albedo.x >= 0);
    assert(albedo.y >= 0);
    assert(albedo.z >= 0);
    assert(albedo.x <= 1);
    assert(albedo.y <= 1);
    assert(albedo.z <= 1);
}

bool material_diffuse_simple::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_point = r.at(hit_t); // hit point, on the surface
    auto nv = hit_obj.normal_vector(hit_point);
    if (dot(nv, r.direction()) > 0) return false; // discard rays from inner (or invert nv)
    auto d = nv + ruvg.normalized();
    if (d.is_zero()) d = nv;
    vec3d diffuse_target = hit_point + nv + ruvg.range01();
    r.decay(albedo); // lose some light when diffused
    r.source(hit_point);
    r.direction((diffuse_target - hit_point).unit_vec()); // the new diffused ray we trace on
    TRACELOG("    reflected (diffusive, simple material)\n");
    return true;
}

material_diffuse_simple::material_diffuse_simple(double albedo) : albedo{albedo, albedo, albedo} {
    assert(albedo >= 0);
    assert(albedo <= 1);
}

material_diffuse_hemispherical::material_diffuse_hemispherical(vec3d albedo) : albedo(albedo) {
    assert(albedo.x >= 0);
    assert(albedo.y >= 0);
    assert(albedo.z >= 0);
    assert(albedo.x <= 1);
    assert(albedo.y <= 1);
    assert(albedo.z <= 1);
}

bool
material_diffuse_hemispherical::scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const {
    const auto hit_point = r.at(hit_t); // hit point, on the surface
    auto nv = hit_obj.normal_vector(hit_point);
    if (dot(nv, r.direction()) > 0) return false; // discard rays from inner (or invert nv)
    auto d = nv + ruvg.normalized();
    if (d.is_zero()) d = nv;
    vec3d diffuse_target = hit_point + ruvg.hemisphere(nv);
    r.decay(albedo); // lose some light when diffused
    r.source(hit_point);
    r.direction((diffuse_target - hit_point).unit_vec()); // the new diffused ray we trace on
    TRACELOG("    reflected (diffusive, hemispherical material)\n");
    return true;
}

material_diffuse_hemispherical::material_diffuse_hemispherical(double albedo) : albedo{albedo, albedo, albedo} {
    assert(albedo >= 0);
    assert(albedo <= 1);
}
