/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::render
{
    class backend
    {
    public:
        virtual ~backend() = default;

        virtual auto begin_frame() -> void = 0;
        virtual auto end_frame() -> void = 0;
    };

    enum class api
    {
        directx,
        opengl,
        vulkan,
    };

    class renderer
    {
    public:
        renderer() = delete;
        renderer(api Api, os::window& Window);

        ~renderer() = default;

        renderer(renderer const& Other) = delete;
        auto operator=(renderer const& Other) -> renderer& = delete;

        renderer(renderer&& Other) noexcept = default;
        auto operator=(renderer&& Other) noexcept -> renderer& = default;

        auto begin_frame() -> void;
        auto end_frame() -> void;

    private:
        os::window& m_Window;
        std::unique_ptr<backend> m_Backend;
    };
} // namespace lambda::render