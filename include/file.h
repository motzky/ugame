#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <memory>
#include <ranges>
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

        template <class T>
        auto write(const T &data) -> void
            requires(sizeof(std::ranges::range_value_t<T>) == 1)
        {
#ifdef _WIN32
            std::memcpy(_map_view.get(), std::ranges::data(data), std::ranges::size(data));
            ensure(::FlushViewOfFile(_map_view.get(), data.size()) != 0, "failed to flush file");

            _filesize = ::GetFileSize(_handle, nullptr);
#else
            std::memcpy(_map_view, std::ranges::data(data), std::ranges::size(data));
            struct stat64 statInfo;
            auto result = fstat64(_handle, &statInfo);
            ensure(result >= 0, "failed to get file size");

            _filesize = statInfo.st_size;
#endif
        }

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