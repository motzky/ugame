#include "file.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string_view>

#ifndef WIN32
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "auto_release.h"
#include "ensure.h"

namespace game
{
    File::File(const std::filesystem::path &path)
        : _handle{-1, ::close},
          _map_view{nullptr, [this](void *view)
                    { ::munmap(view, this->_filesize); }},
          _filesize{0ul}
    {
        _handle.reset(::open(path.string().c_str(), O_RDONLY | O_LARGEFILE));

        ensure(_handle != -1, "failed to open file");
        struct stat64 statInfo;

        auto result = fstat64(_handle, &statInfo);
        ensure(result >= 0, "failed to get file size");

        _filesize = statInfo.st_size;

        _map_view.reset(::mmap64(NULL, _filesize, PROT_READ, MAP_SHARED, _handle, 0ul));
        if (_map_view.get() == MAP_FAILED)
        {
            auto e = errno;
            log::error("{}", e);
        }
        ensure(_map_view.get() != MAP_FAILED, "failed to map file");

        // int linuxHint = 0;
        // switch (_hint)
        // {
        // case Normal:
        //     linuxHint = MADV_NORMAL;
        //     break;
        // case SequentialScan:
        //     linuxHint = MADV_SEQUENTIAL;
        //     break;
        // case RandomAccess:
        //     linuxHint = MADV_RANDOM;
        //     break;
        // default:
        //     break;
        // }
        // // assume that file will be accessed soon
        // // linuxHint |= MADV_WILLNEED;
        // // assume that file will be large
        // // linuxHint |= MADV_HUGEPAGE;

        // ::madvise(_map_view, 0, linuxHint);
    }

    auto File::as_string() const -> std::string_view
    {
        return {reinterpret_cast<const char *>(_map_view.get()), _filesize};
    }

    auto File::as_bytes() const -> std::span<const std::byte>
    {
        return {reinterpret_cast<const std::byte *>(_map_view.get()), _filesize};
    }

}