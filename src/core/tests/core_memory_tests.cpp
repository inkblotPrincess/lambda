/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

using namespace lambda::memory;

namespace
{
    struct non_trivial
    {
        explicit non_trivial(int* Counter)
            : Counter{Counter}
        {

        }

        ~non_trivial()
        {
            ++(*Counter);
        }

        int* Counter;
    };

    struct throwing_constructor
    {
        throwing_constructor()
        {
            throw 123;
        }
    };
}

TEST(core_memory, arena_constructor)
{
    auto Arena = arena{1024};

    EXPECT_EQ(Arena.capacity(), 1024);
    EXPECT_EQ(Arena.used(), 0);
    EXPECT_EQ(Arena.remaining(), 1024);
}

TEST(core_memory, arena_allocate_bytes)
{
    auto Arena = arena{1024};

    auto* Memory = Arena.allocate_bytes(64, alignof(std::max_align_t));

    ASSERT_NE(Memory, nullptr);
    EXPECT_GE(Arena.used(), 64);
    EXPECT_EQ(Arena.remaining(), Arena.capacity() - Arena.used());
}

TEST(core_memory, arena_allocate_bytes_zero_size)
{
    auto Arena = arena{1024};

    auto* Memory = Arena.allocate_bytes(0, alignof(std::max_align_t));

    EXPECT_EQ(Memory, nullptr);
    EXPECT_EQ(Arena.used(), 0);
}

TEST(core_memory, arena_allocate_bytes_over_capacity)
{
    auto Arena = arena{64};

    auto* Memory = Arena.allocate_bytes(128, alignof(std::max_align_t));

    EXPECT_EQ(Memory, nullptr);
    EXPECT_EQ(Arena.used(), 0);
    EXPECT_EQ(Arena.remaining(), 64);
}

TEST(core_memory, arena_allocate_bytes_respects_alignment)
{
    auto Arena = arena{1024};
    constexpr auto Alignment = 32zu;

    auto* Memory = Arena.allocate_bytes(64, Alignment);

    ASSERT_NE(Memory, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(Memory) % Alignment, 0);
}

TEST(core_memory, arena_allocate_array)
{
    auto Arena = arena{1024};

    auto Values = Arena.allocate<int>(4);

    ASSERT_EQ(Values.size(), 4);
    EXPECT_NE(Values.data(), nullptr);

    EXPECT_EQ(Values[0], 0);
    EXPECT_EQ(Values[1], 0);
    EXPECT_EQ(Values[2], 0);
    EXPECT_EQ(Values[3], 0);
}

TEST(core_memory, arena_allocate_array_zero_count)
{
    auto Arena = arena{1024};

    auto Values = Arena.allocate<int>(0);

    EXPECT_TRUE(Values.empty());
    EXPECT_EQ(Values.data(), nullptr);
    EXPECT_EQ(Arena.used(), 0);
}

TEST(core_memory, arena_allocate_array_over_capacity)
{
    auto Arena = arena{16};

    auto Values = Arena.allocate<std::uint64_t>(8);

    EXPECT_TRUE(Values.empty());
    EXPECT_EQ(Values.data(), nullptr);
    EXPECT_EQ(Arena.used(), 0);
}

TEST(core_memory, arena_emplace_trivial)
{
    auto Arena = arena{1024};

    auto* Value = Arena.emplace<int>(42);

    ASSERT_NE(Value, nullptr);
    EXPECT_EQ(*Value, 42);
}

TEST(core_memory, arena_emplace_trivial_over_capacity)
{
    auto Arena = arena{1};

    auto* Value = Arena.emplace<std::uint64_t>(123);

    EXPECT_EQ(Value, nullptr);
    EXPECT_EQ(Arena.used(), 0);
}

TEST(core_memory, arena_emplace_non_trivial)
{
    auto DestroyCount = 0;
    {
        auto Arena = arena{1024};

        auto* Value = Arena.emplace<non_trivial>(&DestroyCount);

        ASSERT_NE(Value, nullptr);
        EXPECT_EQ(DestroyCount, 0);
    }

    EXPECT_EQ(DestroyCount, 1);
}

TEST(core_memory, arena_reset)
{
    auto DestroyCount = 0;
    auto Arena = arena{1024};

    ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);
    ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);

    EXPECT_EQ(DestroyCount, 0);

    Arena.reset();

    EXPECT_EQ(DestroyCount, 2);
    EXPECT_EQ(Arena.used(), 0);
    EXPECT_EQ(Arena.remaining(), Arena.capacity());

    auto* B = Arena.emplace<int>(456);
    ASSERT_NE(B, nullptr);

    EXPECT_EQ(*B, 456);
    EXPECT_EQ(Arena.remaining(), Arena.capacity() - Arena.used());
}

TEST(core_memory, arena_scope)
{
    auto Arena = arena{1024};

    auto* A = Arena.emplace<int>(1);
    ASSERT_NE(A, nullptr);

    auto UsedBeforeScope = Arena.used();

    {
        auto Scope = arena::scope{Arena};

        auto* B = Arena.emplace<int>(2);
        ASSERT_NE(B, nullptr);
        EXPECT_GT(Arena.used(), UsedBeforeScope);
    }

    EXPECT_EQ(Arena.used(), UsedBeforeScope);
}

TEST(core_memory, arena_nested_scopes)
{
    auto DestroyCount = 0;
    auto Arena = arena{1024};

    {
        auto Outer = arena::scope{Arena};

        ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);

        {
            auto Inner = arena::scope{Arena};

            ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);
            ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);
        }

        EXPECT_EQ(DestroyCount, 2);
    }

    EXPECT_EQ(DestroyCount, 3);
    EXPECT_EQ(Arena.used(), 0);
}

TEST(core_memory, arena_emplace_throw)
{
    auto Arena = arena{1024};

    auto UsedBefore = Arena.used();

    EXPECT_THROW({[[maybe_unused]] auto Result = Arena.emplace<throwing_constructor>(); }, int);

    EXPECT_EQ(Arena.used(), UsedBefore);
}

TEST(core_memory, arena_move_constructor)
{
    auto A = arena{1024};

    auto* Value = A.emplace<int>(42);
    ASSERT_NE(Value, nullptr);

    auto B = arena{std::move(A)};

    EXPECT_EQ(B.capacity(), 1024);
    EXPECT_EQ(*Value, 42);
}

TEST(core_memory, arena_move_assignment)
{
    auto DestroyCount = 0;

    auto A = arena{1024};
    ASSERT_NE(A.emplace<non_trivial>(&DestroyCount), nullptr);

    auto B = arena{1024};
    ASSERT_NE(B.emplace<non_trivial>(&DestroyCount), nullptr);

    B = std::move(A);

    EXPECT_EQ(DestroyCount, 1);

    B.reset();

    EXPECT_EQ(DestroyCount, 2);
}