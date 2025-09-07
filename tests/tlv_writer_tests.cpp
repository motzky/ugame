#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "exception.h"
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

TEST(tlv_witer, write_uint32)
{
    auto value = std::uint32_t{0xaabbccdd};
    auto writer = game::TlvWriter{};

    writer.write(value);

    const auto buffer = writer.yield();

    auto reader = game::TlvReader{buffer};
    auto entry = std::ranges::begin(reader);

    ASSERT_EQ(value, (*entry).uint32_value());
}

TEST(tlv_witer, write_string)
{
    auto value = std::string{"hello test"};
    auto writer = game::TlvWriter{};

    writer.write(value);

    const auto buffer = writer.yield();

    auto reader = game::TlvReader{buffer};
    auto entry = std::ranges::begin(reader);

    ASSERT_EQ(value, (*entry).string_value());
}

TEST(tlv_witer, write_byte_array)
{
    auto value = create_binary_vector(0xaa, 0xbb, 0xcc);
    auto writer = game::TlvWriter{};

    writer.write(value);

    const auto buffer = writer.yield();

    auto reader = game::TlvReader{buffer};
    auto entry = std::ranges::begin(reader);

    ASSERT_EQ(value, (*entry).byte_array_value());
}

TEST(tlv_witer, write_texture_format)
{
    auto value = game::TextureFormat::RGBA;
    auto writer = game::TlvWriter{};

    writer.write(value);

    const auto buffer = writer.yield();

    auto reader = game::TlvReader{buffer};
    auto entry = std::ranges::begin(reader);

    ASSERT_EQ(value, (*entry).texture_format_value());
}

TEST(tlv_witer, write_texture_usage)
{
    auto value = game::TextureUsage::SRGB;
    auto writer = game::TlvWriter{};

    writer.write(value);

    const auto buffer = writer.yield();

    auto reader = game::TlvReader{buffer};
    auto entry = std::ranges::begin(reader);

    ASSERT_EQ(value, (*entry).texture_usage_value());
}

TEST(tlv_witer, write_texture_data)
{
    auto data = create_binary_vector(0xaa, 0xbb, 0xcc);
    auto texture_data = game::TextureData{
        .name = "Test texture",
        .format = game::TextureFormat::RGB,
        .usage = game::TextureUsage::DATA,
        .width = 1u,
        .height = 3u,
        .data = data};
    auto writer = game::TlvWriter{};

    writer.write(texture_data);

    const auto buffer = writer.yield();

    auto reader = game::TlvReader{buffer};
    const auto entry = std::ranges::begin(reader);

    const auto tlv_tex = (*entry).texture_data_value();

    ASSERT_EQ(texture_data.name, tlv_tex.name);
    ASSERT_EQ(texture_data.format, tlv_tex.format);
    ASSERT_EQ(texture_data.usage, tlv_tex.usage);
    ASSERT_EQ(texture_data.width, tlv_tex.width);
    ASSERT_EQ(texture_data.height, tlv_tex.height);
    ASSERT_EQ(data, tlv_tex.data);
}
