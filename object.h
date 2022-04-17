//
// Created by Keuin on 2022/4/12.
//

#ifndef RT_OBJECT_H
#define RT_OBJECT_H

#include "material.h"
#include "ray.h"
#include "vec.h"
#include "bitmap.h"
#include <cstdlib>
#include <memory>
#include <limits>
#include <vector>
#include <iostream>
#include <cstdint>

class material;

class object {
public:
    // Will the given ray hit. Returns time t if hits in range [t1, t2].
    virtual bool hit(const ray3d &r, double &t, double t1, double t2) const = 0;

    // With t2 = infinity
    inline bool hit(const ray3d &r, double &t, double t1) const {
        return hit(r, t, t1, std::numeric_limits<double>::infinity());
    }

    // If given position is on the object's surface.
    virtual bool is_on(const vec3d &p) const = 0;

    // Given a point on the surface, returns the normalized outer normal vector on that point.
    virtual vec3d normal_vector(const vec3d &where) const = 0;

    // object color, currently not parameterized
    virtual pixel8b color() const = 0;

    // subclasses must have virtual destructors
    virtual ~object() = default;

    // Get this object's material.
    virtual material &get_material() const = 0;
};


#endif //RT_OBJECT_H
