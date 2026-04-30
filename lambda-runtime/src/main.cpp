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
#include <core/utility/command_line.hpp>

#include <vector>

auto main(int argc, char* argv[]) -> int
{
    using namespace lambda;

    try
    {
        auto Options = utility::parse_options<runtime::application_options>(std::vector<std::string_view>{argv + 1, argv + argc});
        runtime::application_run(Options);
    }
    catch (exception& Ex)
    {
        log::fatal("lambda::exception: {}", Ex.what());
        return EXIT_FAILURE;
    }
    catch (std::exception& Ex)
    {
        log::fatal("std::exception: {}", Ex.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        log::fatal("Unrecognised exception");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}