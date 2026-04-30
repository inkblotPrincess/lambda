/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

#pragma once

#include <core/base/context.hpp>

#include <concepts>

#if defined(LAMBDA_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    #include <gl/GL.h>
#endif

#define GL_X_MACRO(GL_MACRO) \
    GL_MACRO(GenVertexArrays, void, (GLsizei n, GLuint* arrays))

namespace lambda::render::opengl
{
    struct vtable
    {
        #define GL_DEFINE_VTABLE_FUNC(Name, ReturnType, Parameters) \
            using Name##_type = ReturnType(APIENTRY*) Parameters; \
            Name##_type Name = nullptr;
        GL_X_MACRO(GL_DEFINE_VTABLE_FUNC)
        #undef GL_DEFINE_VTABLE_FUNC

        template<class func_type>
        requires std::invocable<func_type&, char const*> && std::same_as<std::invoke_result_t<func_type&, char const*>, void*>
        vtable(func_type&& LoaderFunc)
        {
            #define GL_LOAD_VTABLE_FUNC(Name, ...) Name = reinterpret_cast<Name##_type>(std::invoke(std::forward<func_type>(LoaderFunc), "gl"#Name));
            GL_X_MACRO(GL_LOAD_VTABLE_FUNC)
            #undef GL_LOAD_VTABLE_FUNC
        }

        ~vtable() = default;

        vtable(vtable const& Other) = delete;
        auto operator=(vtable const& Other) -> vtable& = delete;

        vtable(vtable&& Other) noexcept = delete;
        auto operator=(vtable&&) noexcept -> vtable& = delete;
    };
} // namespace lambda::render::opengl