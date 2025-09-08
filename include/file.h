#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string_view>

#ifdef WIN32
#include <Windows>
#else
#include <limits>
#include <sys/mman.h>
#endif

#include "auto_release.h"

namespace game
{
    class File
    {
    public:
        File(const std::filesystem::path &path);

        auto as_string() const -> std::string_view;
        auto as_bytes() const -> std::span<const std::byte>;

    private:
#ifdef _WIN32
        using FileHandle = HANDLE;
        AutoRelease<HANDLE, INVALID_HANDLE_VALUE> _handle;
        AutoRelease<HANDLE, reinterpret_cast<HANDLE>(NULL)> _mapping;
        std::unique_ptr<void, decltype(&::UnmapViewOfFile)> _map_view;
#else
        using FileHandle = int;
        AutoRelease<int, -1> _handle;
        AutoRelease<void *, nullptr> _map_view;
        std::size_t _filesize;
#endif
    };
}