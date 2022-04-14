//
// Created by Keuin on 2022/4/14.
//

#include <gtest/gtest.h>

#include "bitmap.h"

TEST(Pixel, PixelDepthConvert) {
    const auto white8 = pixel8b::white();
    const auto white32 = pixel<uint32_t>::white();
    ASSERT_EQ(white8, pixel8b::from(white32));

    const auto black8 = pixel8b::black();
    const auto black32 = pixel<uint32_t>::black();
    ASSERT_EQ(black8, pixel8b::from(black32));

    const auto gray8 = pixel8b{127, 127, 127};
    const auto gray32 = pixel<uint32_t>{
            ((1ULL << 32U) - 1U) >> 1U,
            ((1ULL << 32U) - 1U) >> 1U,
            ((1ULL << 32U) - 1U) >> 1U
    };
    ASSERT_EQ(gray8, pixel8b::from(gray32));

    const auto mix8 = pixel8b{0, 127, 255};
    const auto mix32 = pixel<uint32_t>{
            0,
            ((1ULL << 32U) - 1U) >> 1U,
            ((1ULL << 32U) - 1U)
    };
    ASSERT_EQ(mix8, pixel8b::from(mix32));
}