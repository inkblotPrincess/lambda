/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/base/error.hpp>

#include <algorithm>
#include <meta>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <span>

namespace lambda::utility
{
    template<class options_type>
    auto parse_options(std::span<std::string_view const> Arguments) -> options_type
    {
        auto Options = options_type{};

        static constexpr auto Members = std::define_static_array(std::meta::nonstatic_data_members_of(^^options_type, std::meta::access_context::current()));
        template for (constexpr auto Member : Members)
        {
            auto It = std::ranges::find_if(Arguments, [](std::string_view Arg) {
                return Arg.starts_with("--") && Arg.substr(2) == std::meta::identifier_of(Member);
            });

            if (It == Arguments.end())
                continue;

            if (std::next(It) == Arguments.end())
                throw exception{"Missing option value for {}", *It};

            using member_type = [:std::meta::type_of(Member):];
            auto Value = *std::next(It);

            if constexpr (std::same_as<member_type, std::string>)
            {
                Options.[:Member:] = std::string{Value};
            }
            else if constexpr (std::same_as<member_type, std::string_view>)
            {
                Options.[:Member:] = Value;
            }
            else
            {
                std::istringstream Stream{std::string{Value}};
                if (!(Stream >> Options.[:Member:]))
                    throw exception{"Failed to parse option {} to {}", *It, std::meta::display_string_of(^^member_type)};
            }
        }

        return Options;
    }
} // namespace lambda::utility