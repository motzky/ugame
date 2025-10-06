#include "scripting/script_loader.h"

#include "file.h"
#include "tlv/tlv_reader.h"

namespace game
{
    ScriptLoader::ScriptLoader(const std::string &name)
        : _name{name},
          _reader{}
    {
    }

    ScriptLoader::ScriptLoader(const std::string &name, const TlvReader &reader)
        : _name{name},
          _reader{std::addressof(reader)}
    {
    }

    auto ScriptLoader::name() const -> std::string
    {
        return _name;
    }

    auto ScriptLoader::load() const -> std::string
    {
        if (_reader)
        {
            return TlvReader::get_text_file(**_reader, _name).data;
        }

        const auto file = File{_name};
        return std::string{file.as_string()};
    }
}
