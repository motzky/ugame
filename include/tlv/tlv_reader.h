#pragma once

#include <cstddef>
#include <iterator>
#include <span>

#include "graphics/mesh_data.h"
#include "graphics/texture.h"
#include "tlv/tlv_entry.h"

namespace game
{
    class TlvReader
    {
    public:
        class Iterator
        {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = TlvEntry;

            Iterator() = default;
            Iterator(std::span<const std::byte> buffer);

            auto operator*() const -> value_type;
            auto operator++() -> Iterator &;
            auto operator++(int) -> Iterator;
            auto operator==(const Iterator &) const -> bool;

        private:
            std::span<const std::byte> _buffer;
        };

        static_assert(std::forward_iterator<Iterator>);

        TlvReader(std::span<const std::byte> buffer);

        auto begin(this auto &&self) -> Iterator
        {
            return {self._buffer};
        }

        auto end(this auto &&self) -> Iterator
        {
            return {{self._buffer.data() + self._buffer.size(), self._buffer.data() + self._buffer.size()}};
        }

        static auto get_text_file(const TlvReader &reader, std::string_view name) -> TextFile;

    private:
        std::span<const std::byte> _buffer;
    };

}
