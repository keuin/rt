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

TEST(Vec, VecMin) {
    vec3i a{1, 1, 1}, b{2, 2, 2}, c{-1, -1, -1};
    ASSERT_EQ(a - b, c);
    ASSERT_EQ(-a, c);

    vec3d d{1.1, 2.2, 3.3}, e{-4.4, -5.5, -6.6}, f{5.5, 7.7, 9.9};
    ASSERT_EQ(d - e, f);
    ASSERT_EQ(d + (-e), f);
}

TEST(Vec, VecCrossProduct) {
    vec3i a{1, 1, 1}, b{2, 2, 2}, c{3, 4, 5}, d{6, 7, 8}, e{-3, -6, -3};
    ASSERT_EQ(a * b, vec3i{});
    ASSERT_EQ(c * d, e);

    vec3d f{3.0, 4.0, 5.0}, g{6.0, 7.0, 8.0}, h{-3, -6, -3};
    ASSERT_EQ(f * g, h);
}