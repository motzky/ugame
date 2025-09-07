#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "exception.h"
#include "tlv_entry.h"
#include "tlv_reader.h"

namespace
{
    template <class... Args>
    auto create_binary_vector(Args... args) -> std::vector<std::byte>
    {
        return {std::byte(args)...};
    }
}

TEST(tlv_entry, ctor)
{
    const auto entry = game::TlvEntry{game::TlvType::UINT32, {}};
    ASSERT_EQ(entry.type(), game::TlvType::UINT32);
}

TEST(tlv_entry, uint32_value)
{

    const auto bytes = std::vector<std::byte>{create_binary_vector(0xdd, 0xcc, 0xbb, 0xaa)};
    const auto entry = game::TlvEntry{game::TlvType::UINT32, bytes};

    ASSERT_EQ(entry.type(), game::TlvType::UINT32);
    ASSERT_EQ(entry.uint32_value(), 0xaabbccdd);
}

TEST(tlv_entry, uint32_value_size)
{

    const auto bytes = std::vector<std::byte>{create_binary_vector(0xdd, 0xcc, 0xbb, 0xaa)};
    const auto entry = game::TlvEntry{game::TlvType::UINT32, bytes};

    ASSERT_EQ(entry.type(), game::TlvType::UINT32);
    ASSERT_EQ(entry.uint32_value(), 0xaabbccdd);
}

TEST(tlv_entry, uint32_value_invalid_type)
{

    const auto bytes = std::vector<std::byte>{create_binary_vector(0xdd, 0xcc, 0xbb)};
    const auto entry = game::TlvEntry{game::TlvType::UINT32, bytes};

    ASSERT_THROW(entry.uint32_value(), game::Exception);
}

TEST(tlv_entry, string_value)
{

    const auto bytes = std::vector<std::byte>{create_binary_vector('h', 'e', 'l', 'l', 'o')};
    const auto entry = game::TlvEntry{game::TlvType::STRING, bytes};

    ASSERT_EQ(entry.type(), game::TlvType::STRING);
    ASSERT_EQ(entry.string_value(), "hello");
}

TEST(tlv_entry, string_value_invalid_type)
{
    const auto bytes = std::vector<std::byte>{create_binary_vector('h', 'e', 'l', 'l', 'o')};
    const auto entry = game::TlvEntry{game::TlvType::UINT32, bytes};

    ASSERT_THROW(entry.string_value(), game::Exception);
}

TEST(tlv_entry, byte_array_value)
{
    const auto bytes = std::vector<std::byte>{create_binary_vector(0xaa, 0xbb, 0xcc)};
    const auto entry = game::TlvEntry{game::TlvType::BYTE_ARRAY, bytes};

    ASSERT_EQ(entry.byte_array_value(), bytes);
}

TEST(tlv_entry, byte_array_value_invalid)
{
    const auto bytes = std::vector<std::byte>{create_binary_vector(0xaa, 0xbb, 0xcc)};
    const auto entry = game::TlvEntry{game::TlvType::UINT32, bytes};

    ASSERT_THROW(entry.byte_array_value(), game::Exception);
}
