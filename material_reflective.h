//
// Created by Keuin on 2022/4/15.
//

#ifndef RT_MATERIAL_REFLECTIVE_H
#define RT_MATERIAL_REFLECTIVE_H

#include "vec.h"
#include "material.h"

template<bool cond>
struct s_fuzzy_ {
    // non-fuzzy material does not need these variables
    // we remove them with an empty struct
};

template<>
struct s_fuzzy_<true> {
    double f; // how fuzzy the material is
    // those are parameters that only fuzzy materials have
};

// metal with perfectly smooth surface
template<bool Fuzzy>
class material_reflective_ : public material {
    vec3d albedo;
    s_fuzzy_<Fuzzy> fuzzy_;
public:
    // FIXME conditionally enable these constructors by template parameter `Fuzzy`.

    // Non-fuzzy constructors
    explicit material_reflective_(vec3d &color) : albedo(color), fuzzy_{} {}

    explicit material_reflective_(vec3d &&color) : albedo(color), fuzzy_{} {}

    explicit material_reflective_(double color) : albedo{color, color, color}, fuzzy_{} {}

    // Fuzzy constructors, with special parameters
    explicit material_reflective_(vec3d &color, double f) : albedo(color), fuzzy_{f} {}

    explicit material_reflective_(vec3d &&color, double f) : albedo(color), fuzzy_{f} {}

    explicit material_reflective_(double color, double f) : albedo{color, color, color}, fuzzy_{f} {}

    bool scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const override;
};

using material_reflective = material_reflective_<false>;
using material_fuzzy_reflective = material_reflective_<true>;

#endif //RT_MATERIAL_REFLECTIVE_H
