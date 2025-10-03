#include <gtest/gtest.h>

#include <cstddef>
#include <ranges>
#include <string_view>
#include <vector>

#include "utils/compress.h"
#include "utils/decompress.h"

#include "test_utils.h"

using namespace std::string_view_literals;

TEST(compress, round_trip)
{
    TEST_IMPL(
        const auto text = R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.)"sv;

        const auto text_as_bytes = text |
                                   std::views::transform([](auto c)
                                                         { return static_cast<std::byte>(c); }) |
                                   std::ranges::to<std::vector>();

        const auto compressed = game::compress(text_as_bytes);

        const auto decompressed = game::decompress(compressed);

        ASSERT_NE(compressed, decompressed);
        ASSERT_TRUE(std::ranges::equal(decompressed, text_as_bytes));

    )
}