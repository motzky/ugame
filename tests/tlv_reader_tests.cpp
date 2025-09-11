#include <cstddef>
#include <cstdint>
#include <ranges>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "core/exception.h"
#include "tlv/tlv_entry.h"
#include "tlv/tlv_reader.h"

namespace
{
    template <class... Args>
    auto create_binary_vector(Args... args) -> std::vector<std::byte>
    {
        return {std::byte(args)...};
    }
}

TEST(tlv_reader, begin)
{
    const auto bytes = create_binary_vector(
        0x00, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00,
        0xdd, 0xcc, 0xbb, 0xaa);

    const auto reader = game::TlvReader{{bytes}};
    const auto begin = reader.begin();

    ASSERT_EQ((*begin).type(), game::TlvType::UINT32);
    ASSERT_EQ((*begin).uint32_value(), 0xaabbccdd);
}

TEST(tlv_reader, range_loop)
{
    const auto bytes = create_binary_vector(
        std::to_underlying(game::TlvType::UINT32), 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00,
        0xdd, 0xcc, 0xbb, 0xaa,
        std::to_underlying(game::TlvType::STRING), 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00,
        'y', 'o');

    const auto reader = game::TlvReader{{bytes}};

    for (const auto &[index, entry] : reader | std::views::enumerate)
    {
        if (index == 0u)
        {
            ASSERT_EQ(entry.type(), game::TlvType::UINT32);
            ASSERT_EQ(entry.uint32_value(), 0xaabbccdd);
        }
        else if (index == 1u)
        {
            ASSERT_EQ(entry.type(), game::TlvType::STRING);
            ASSERT_EQ(entry.string_value(), "yo");
        }
        else
        {
            ASSERT_TRUE(false);
        }
    }
}
