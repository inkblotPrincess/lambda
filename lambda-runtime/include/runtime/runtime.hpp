/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <cstddef>
#include <string>

namespace lambda::runtime
{
    struct application_options
    {
        std::string WindowTitle = "Lambda :3";
    };

    auto application_run(application_options const& Options) -> void;
} // namespace lambda::runtime