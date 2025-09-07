#include "tlv/tlv_reader.h"

#include <cstddef>
#include <cstring>
#include <iterator>
#include <span>

#include "ensure.h"

namespace game
{
    TlvReader::TlvReader(std::span<const std::byte> buffer)
        : _buffer{buffer}
    {
    }

    TlvReader::Iterator::Iterator(std::span<const std::byte> buffer)
        : _buffer(buffer)
    {
    }

    auto TlvReader::Iterator::operator*() const -> TlvReader::Iterator::value_type
    {
        const auto remaining_byes = _buffer.size();
        ensure(remaining_byes >= sizeof(TlvType) + sizeof(std::uint32_t), "invalid entry size {}", remaining_byes);

        auto type = TlvType{};
        std::memcpy(&type, _buffer.data(), sizeof(type));

        auto length = std::uint32_t{};
        std::memcpy(&length, _buffer.data() + sizeof(type), sizeof(length));

        // const auto needed_bytes = sizeof(TlvType) + sizeof(std::uint32_t) + length;
        // ensure(remaining_byes >= sizeof(TlvType) + sizeof(std::uint32_t) + length, "invalid entry size. Remaining {}, need {}", remaining_byes, needed_bytes);

        return {type, _buffer.subspan(sizeof(type) + sizeof(length), length)};
    }

    auto TlvReader::Iterator::operator++() -> TlvReader::Iterator &
    {
        const auto entry = operator*();
        _buffer = _buffer.last(_buffer.size() - entry.size());
        return *this;
    }

    auto TlvReader::Iterator::operator++(int) -> TlvReader::Iterator
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    auto TlvReader::Iterator::operator==(const Iterator &other) const -> bool
    {
        return (other._buffer.size() == _buffer.size()) && (other._buffer.data() == _buffer.data());
    }
}
