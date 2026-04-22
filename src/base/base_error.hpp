/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

namespace lambda
{
	template<class... arg_types>
	auto expect(bool Predicate, std::format_string<arg_types...> Format, arg_types&&... Arguments) -> void
	{
		if (!Predicate)
		{
			log::fatal(Format, std::forward<arg_types>(Arguments)...);
			log::fatal(" --> {}", std::stacktrace::current(1));
			
			std::terminate();
		}
	}
} // namespace lambda