/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <core/math/vector.hpp>

#include <gtest/gtest.h>

TEST(vec2, constructor)
{
    using namespace lambda::math;

    EXPECT_EQ(vec2i(), vec2i(0, 0));
    EXPECT_EQ(vec2i(1, 2), vec2i(1, 2));
    EXPECT_EQ(vec2i(7), vec2i(7, 7));
}

TEST(vec2, dot)
{
    using namespace lambda::math;

    EXPECT_EQ(vec2i::dot(vec2i(1, 2), vec2i(3, 4)), 11);
}

TEST(vec2, length)
{
    using namespace lambda::math;

    EXPECT_FLOAT_EQ(vec2f(3.0f, 4.0f).length(), 5.0f);
}

TEST(vec2, arithmetic)
{
    using namespace lambda::math;

    auto A = vec2i{8, 12};
    auto B = vec2i{2, 4};

    EXPECT_EQ(A + B, vec2i(10, 16));
    EXPECT_EQ(A - B, vec2i(6, 8));
    EXPECT_EQ(A * B, vec2i(16, 48));
    EXPECT_EQ(A / B, vec2i(4, 3));

    EXPECT_EQ(A * 2, vec2i(16, 24));
    EXPECT_EQ(2 * A, vec2i(16, 24));
    EXPECT_EQ(A / 2, vec2i(4, 6));

    EXPECT_EQ(+A, vec2i(8, 12));
    EXPECT_EQ(-A, vec2i(-8, -12));
}

TEST(vec2, compound)
{
    using namespace lambda::math;

    auto V = vec2i{8, 12};

    V += vec2i{1, 2};
    EXPECT_EQ(V, vec2i(9, 14));

    V -= vec2i{4, 4};
    EXPECT_EQ(V, vec2i(5, 10));

    V *= vec2i{2, 3};
    EXPECT_EQ(V, vec2i(10, 30));

    V /= vec2i{2, 5};
    EXPECT_EQ(V, vec2i(5, 6));

    V *= 3;
    EXPECT_EQ(V, vec2i(15, 18));

    V /= 3;
    EXPECT_EQ(V, vec2i(5, 6));
}

TEST(vec2, equality)
{
    using namespace lambda::math;

    EXPECT_EQ(vec2i(1, 2), vec2i(1, 2));
    EXPECT_NE(vec2i(1, 2), vec2i(2, 1));
}

TEST(vec3, constructor)
{
    using namespace lambda::math;

    EXPECT_EQ(vec3i(), vec3i(0, 0, 0));
    EXPECT_EQ(vec3i(1, 2, 3), vec3i(1, 2, 3));
    EXPECT_EQ(vec3i(1, vec2i(2, 3)), vec3i(1, 2, 3));
    EXPECT_EQ(vec3i(vec2i(1, 2), 3), vec3i(1, 2, 3));
    EXPECT_EQ(vec3i(7), vec3i(7, 7, 7));
}

TEST(vec3, dot)
{
    using namespace lambda::math;

    EXPECT_EQ(vec3i::dot(vec3i(1, 2, 3), vec3i(4, 5, 6)), 32);
}

TEST(vec2, cross)
{
    using namespace lambda::math;

    EXPECT_EQ(vec3i::cross(vec3i(1, 0, 0), vec3i(0, 1, 0)), vec3i(0, 0, 1));
}

TEST(vec3, length)
{
    using namespace lambda::math;

    EXPECT_FLOAT_EQ(vec3f(2.0f, 3.0f, 6.0f).length(), 7.0f);
}

TEST(vec3, swizzles)
{
    using namespace lambda::math;

    auto V = vec3i{1, 2, 3};

    EXPECT_EQ(V.xy(), vec2i(1, 2));
    EXPECT_EQ(V.yz(), vec2i(2, 3));
}

TEST(vec3, arithmetic)
{
    using namespace lambda::math;

    auto A = vec3i{8, 12, 16};
    auto B = vec3i{2, 4, 8};

    EXPECT_EQ(A + B, vec3i(10, 16, 24));
    EXPECT_EQ(A - B, vec3i(6, 8, 8));
    EXPECT_EQ(A * B, vec3i(16, 48, 128));
    EXPECT_EQ(A / B, vec3i(4, 3, 2));

    EXPECT_EQ(A * 2, vec3i(16, 24, 32));
    EXPECT_EQ(2 * A, vec3i(16, 24, 32));
    EXPECT_EQ(A / 2, vec3i(4, 6, 8));

    EXPECT_EQ(+A, vec3i(8, 12, 16));
    EXPECT_EQ(-A, vec3i(-8, -12, -16));
}

TEST(vec3, equality)
{
    using namespace lambda::math;

    EXPECT_EQ(vec3i(1, 2, 3), vec3i(1, 2, 3));
    EXPECT_NE(vec3i(1, 2, 3), vec3i(3, 2, 1));
}

TEST(vec4, constructor)
{
    using namespace lambda::math;

    EXPECT_EQ(vec4i(), vec4i(0, 0, 0, 0));
    EXPECT_EQ(vec4i(1, 2, 3, 4), vec4i(1, 2, 3, 4));
    EXPECT_EQ(vec4i(1, 2, vec2i(3, 4)), vec4i(1, 2, 3, 4));
    EXPECT_EQ(vec4i(1, vec2i(2, 3), 4), vec4i(1, 2, 3, 4));
    EXPECT_EQ(vec4i(vec2i(1,2), 3, 4), vec4i(1, 2, 3, 4));
    EXPECT_EQ(vec4i(vec2i(1, 2), vec2i(3, 4)), vec4i(1, 2, 3, 4));
    EXPECT_EQ(vec4i(1, vec3i(2, 3, 4)), vec4i(1, 2, 3, 4));
    EXPECT_EQ(vec4i(vec3i(1, 2, 3), 4), vec4i(1, 2, 3, 4));
    EXPECT_EQ(vec4i(7), vec4i(7, 7, 7, 7));
}

TEST(vec4, dot)
{
    using namespace lambda::math;

    EXPECT_EQ(vec4i::dot(vec4i(1, 2, 3, 4), vec4i(5, 6, 7, 8)), 70);
}

TEST(vec4, length)
{
    using namespace lambda::math;

    EXPECT_FLOAT_EQ(vec4f(1.0f, 2.0f, 4.0f, 8.0f).length(), std::sqrt(85.0f));
}

TEST(vec4, swizzles)
{
    using namespace lambda::math;

    auto V = vec4i{1, 2, 3, 4};

    EXPECT_EQ(V.xy(), vec2i(1, 2));
    EXPECT_EQ(V.yz(), vec2i(2, 3));
    EXPECT_EQ(V.zw(), vec2i(3, 4));

    EXPECT_EQ(V.xyz(), vec3i(1, 2, 3));
    EXPECT_EQ(V.yzw(), vec3i(2, 3, 4));
}

TEST(vec4, arithmetic)
{
    using namespace lambda::math;

    auto A = vec4i{8, 12, 16, 20};
    auto B = vec4i{2, 4, 8, 10};

    EXPECT_EQ(A + B, vec4i(10, 16, 24, 30));
    EXPECT_EQ(A - B, vec4i(6, 8, 8, 10));
    EXPECT_EQ(A * B, vec4i(16, 48, 128, 200));
    EXPECT_EQ(A / B, vec4i(4, 3, 2, 2));

    EXPECT_EQ(A * 2, vec4i(16, 24, 32, 40));
    EXPECT_EQ(2 * A, vec4i(16, 24, 32, 40));
    EXPECT_EQ(A / 2, vec4i(4, 6, 8, 10));

    EXPECT_EQ(+A, vec4i(8, 12, 16, 20));
    EXPECT_EQ(-A, vec4i(-8, -12, -16, -20));
}

TEST(vec4, equality)
{
    using namespace lambda::math;

    EXPECT_EQ(vec4i(1, 2, 3, 4), vec4i(1, 2, 3, 4));
    EXPECT_NE(vec4i(1, 2, 3, 4), vec4i(4, 3, 2, 1));
}