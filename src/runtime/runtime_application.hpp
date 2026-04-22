/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda::runtime
{
    struct command_line_arguments
    {
        std::size_t Count;
        char** Arguments;
    };

    auto application_run(command_line_arguments const& Arguments) -> void;
} // namespace lambda::runtime