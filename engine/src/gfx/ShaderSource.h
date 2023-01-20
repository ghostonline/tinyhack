#pragma once

#include "RenderConstants.h"

#include <ds/StringView.h>

#include <string>
#include <vector>

struct ShaderSource
{
    ShaderSource(const StringView& name, const StringView& vertex_program, const StringView& fragment_program)
    : program_name(name.get_ptr(), name.get_size())
    , vertex_program(vertex_program.get_ptr(), vertex_program.get_size())
    , fragment_program(fragment_program.get_ptr(), fragment_program.get_size())
    {}

    std::string program_name;
    std::string vertex_program;
    std::string fragment_program;
};
