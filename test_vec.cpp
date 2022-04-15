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

TEST(Vec, CrossProduct) {
    vec3i a{1, 1, 1}, b{2, 2, 2}, c{3, 4, 5}, d{6, 7, 8}, e{-3, -6, -3};
    ASSERT_EQ(cross(a, b), vec3i{});
    ASSERT_EQ(a.cross(b), vec3i{});
    ASSERT_EQ(cross(c, d), e);
    ASSERT_EQ(c.cross(d), e);

    vec3d f{3, 4, 5}, g{6, 7, 8}, h{-3, -6, -3};
    ASSERT_EQ(cross(f, g), h);
    ASSERT_EQ(f.cross(g), h);
}

TEST(Vec, DotProduct) {
    vec3i c{3, 4, 5}, d{6, 7, 8};
    int e = 18 + 28 + 40;
    ASSERT_EQ(dot(c, d), e);
    ASSERT_EQ(c.dot(d), e);

    vec3d f{3, 4, 5}, g{6, 7, 8};
    ASSERT_EQ(dot(f, g), e);
    ASSERT_EQ(f.dot(g), e);
}

TEST(Vec, MulByScalar) {
    vec3i a{1, 1, 1}, b{2, 2, 2};
    vec3d c{7, 14, 21}, d{1, 2, 3};
    ASSERT_EQ(a * 2, b);
    ASSERT_EQ(a / 0.5, b);
    ASSERT_EQ(2 * a, b);
    ASSERT_EQ(c * (1.0 / 7), d);
    ASSERT_EQ(c / 7, d);
    ASSERT_EQ(c / 7.0, d);
}

TEST(Vec, Norm) {
    vec3d a{1, 1, 1}, b{2, 2, 2};
    ASSERT_EQ(a.norm(), sqrt(3));
    ASSERT_EQ(b.norm(), sqrt(12));
}

TEST(Vec, UnitVec) {
    vec3d a{1, 2, 2}, b{1.0 / 3, 2.0 / 3, 2.0 / 3};
    ASSERT_EQ(a.unit_vec(), b);
}

TEST(Vec, Mod2) {
    vec3i a{1, 2, 3};
    vec3d b{2.5, 3, 1.2};
    ASSERT_EQ(a.mod2(), 14);
    ASSERT_LE(abs(b.mod2() - (2.5 * 2.5 + 3 * 3 + 1.2 * 1.2)), 1e-10);
}

TEST(Vec, IsZero) {
    vec3i a{1,0,0}, b{0,-1,0}, c{1,2,3};
    ASSERT_FALSE(a.is_zero());
    ASSERT_FALSE(b.is_zero());
    ASSERT_FALSE(c.is_zero());

    vec3d d{0.1,0,0}, e{0,-0.1,0},f{0.1,0.1,0.1};
    ASSERT_FALSE(d.is_zero());
    ASSERT_FALSE(e.is_zero());
    ASSERT_FALSE(f.is_zero());

    vec3i g{0,0,0};
    vec3d h{0,0,0}, i{1e-10,0,0}, j{1e-10,1e-10,1e-10};
    ASSERT_TRUE(g.is_zero());
    ASSERT_TRUE(h.is_zero());
    ASSERT_TRUE(i.is_zero());
    ASSERT_TRUE(j.is_zero());
}

TEST(Vec, Reflect) {
    vec3d n{1,0,0}, u{-1,1.1,0}, v{1,1.1,0};
    ASSERT_EQ(v, n.reflect(u));
}