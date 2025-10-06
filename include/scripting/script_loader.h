#pragma once

#include <optional>
#include <string>

#include "tlv/tlv_reader.h"

namespace game
{
    class ScriptLoader
    {
    public:
        ScriptLoader(const std::string &name);
        ScriptLoader(const std::string &name, const TlvReader &reader);

        auto name() const -> std::string;
        auto load() const -> std::string;

    private:
        std::string _name;
        std::optional<const TlvReader *> _reader;
    };

}