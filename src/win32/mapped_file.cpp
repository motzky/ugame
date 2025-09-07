#ifndef _WIN32
#error This code unit is ONLY for Windows !
#endif

#include "mapped_file.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string_view>

#ifdef WIN32
#include <Windows>
#endif

#include "auto_release.h"
#include "ensure.h"

namespace game
{
    MappedFile::MappedFile(const std::filesystem::path &path)
        : _handle(INVALID_HANDLE_VALUE, ::CloseHandle),
          _mapping(NULL, ::CloaseHandle),
          _map_view{nullptr, ::UnmapViewOfFile},
          _filesize{}
    {
        _handle.reset(::CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));

        ensure(_handle, "failed to open file");

        _mapping.reset(::CreateFileMappingA(_handle, nullptr, PAGE_READWRITE, 0, 0, nullptr));

        ensure(_mapping, "failed to map file");

        _map_view.reset(::MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0));
        ensure(_map_view, "failed to get map view");

        _filesize = ::GetFileSize(_handle, nullptr);
    }

    auto MappedFile::as_string() const -> std::string_view
    {
        return {reinterpret_cast<const char *>(_map_view.get()), _filesize};
    }

    auto MappedFile::as_bytes() const -> std::span<const std::byte>
    {
        return {reinterpret_cast<const std::byte *>(_map_view.get()), _filesize};
    }