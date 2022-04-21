//
// Created by Keuin on 2022/4/15.
//

#ifndef RT_MATERIAL_H
#define RT_MATERIAL_H

#include "vec.h"
#include "object.h"

class object;

// A material is of some characteristics that manipulates the lighted ray in some certain ways.
// An object can have one certain subclass of material, making it seemed to be different with other objects.
// A material instance can be configured when constructing. It should be immutable and thread-safe.
// It is suggested not to have mutable states in its internal implementation.
// User should have a global table containing references to all the materials the scene is using.
class material {
public:
    // If this matter scatters, manipulate r and return true. Otherwise return false (the light is absorbed entirely).
    // To make it thread-safe without locking mechanism, private states are passed-in as parameters.
    virtual bool scatter(ray3d &r, const object &hit_obj, double hit_t, random_uv_gen_3d &ruvg) const = 0;

    virtual ~material() = default;
};

#endif //RT_MATERIAL_H
