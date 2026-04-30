/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#include <runtime/runtime.hpp>

#include <core/base/error.hpp>
#include <core/base/logging.hpp>

auto main(int argc, char* argv[]) -> int
{
    try
    {
        auto Arguments = lambda::runtime::command_line_arguments{
            .Count     = static_cast<std::size_t>(argc + 1),
            .Arguments = argc > 1 ? argv + 1 : nullptr
        };

        lambda::runtime::application_run(Arguments);
    }
    catch (lambda::exception& Ex)
    {
        lambda::log::fatal("lambda::exception: {}", Ex.what());
        return EXIT_FAILURE;
    }
    catch (std::exception& Ex)
    {
        lambda::log::fatal("std::exception: {}", Ex.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        lambda::log::fatal("Unrecognised exception");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}