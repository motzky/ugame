#include <cstddef>
#include <cstdint>
#include <print>
#include <span>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "core/exception.h"
#include "tlv/tlv_entry.h"
#include "tlv/tlv_reader.h"
#include "tlv/tlv_writer.h"

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

TEST(tlv_entry, byte_array_value_invalid_type)
{
    const auto bytes = std::vector<std::byte>{create_binary_vector(0xaa, 0xbb, 0xcc)};
    const auto entry = game::TlvEntry{game::TlvType::UINT32, bytes};

    ASSERT_THROW(entry.byte_array_value(), game::Exception);
}

TEST(tlv_entry, texture_value)
{
    auto data = create_binary_vector(
        0x05, 0x00, 0x00, 0x00,
        0x4f, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00,
        0x0c, 0x00, 0x00, 0x00,
        0x54, 0x65, 0x73, 0x74,
        0x20, 0x74, 0x65, 0x78,
        0x74, 0x75, 0x72, 0x65,
        0x00, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00,
        0xaa, 0xbb, 0xcc);

    auto reader = game::TlvReader{data};
    const auto entry = std::ranges::begin(reader);

    const auto tlv_tex = (*entry).texture_description_value();

    const auto expected_bytes = std::vector<std::byte>{create_binary_vector(0xaa, 0xbb, 0xcc)};
    ASSERT_EQ(tlv_tex.name, "Test texture");
    ASSERT_EQ(tlv_tex.format, game::TextureFormat::RGB);
    ASSERT_EQ(tlv_tex.usage, game::TextureUsage::DATA);
    ASSERT_EQ(tlv_tex.width, 1u);
    ASSERT_EQ(tlv_tex.height, 3u);
    ASSERT_EQ(tlv_tex.data, expected_bytes);
}

TEST(tlv_entry, texture_value_invalid_type)
{
    const auto bytes = std::vector<std::byte>{create_binary_vector(0xaa, 0xbb, 0xcc)};
    const auto entry = game::TlvEntry{game::TlvType::UINT32, bytes};

    ASSERT_THROW(entry.byte_array_value(), game::Exception);
}
