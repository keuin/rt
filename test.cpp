//
// Created by Keuin on 2022/4/11.
//

#include <gtest/gtest.h>

#include "vec.h"

TEST(Vec, VecAdd) {
    vec3i a{1, 1, 1}, b{2, 2, 2}, c{3, 3, 3};
    ASSERT_EQ(a + b, c);

    vec3d d{1.1, 2.2, 3.3}, e{4.4, 5.5, 6.6}, f{5.5, 7.7, 9.9};
    ASSERT_EQ(d + e, f);
}