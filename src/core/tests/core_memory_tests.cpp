/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace
{
    struct non_trivial
    {
        explicit non_trivial(std::size_t* Counter) noexcept
            : Counter{Counter}
        {

        }

        ~non_trivial()
        {
            ++(*Counter);
        }

        std::size_t* Counter;
    };

    struct throwing_constructor
    {
        throwing_constructor()
        {
            throw 123;
        }
    };
}

TEST(arena, constructor)
{
    using namespace lambda::memory;

    auto const Arena = arena{1024zu};

    EXPECT_EQ(Arena.capacity(), 1024zu);
    EXPECT_EQ(Arena.remaining(), 1024zu);
    EXPECT_EQ(Arena.used(), 0zu);
}

TEST(arena, move_constructor)
{
    using namespace lambda::memory;

    auto A = arena{1024zu};

    auto* const Value = A.emplace<int>(42);
    ASSERT_NE(Value, nullptr);

    auto const B = arena{std::move(A)};

    EXPECT_EQ(B.capacity(), 1024);
    EXPECT_EQ(*Value, 42);
}

TEST(arena, move_assignment)
{
    using namespace lambda::memory;

    auto DestroyCount = 0zu;

    auto A = arena{1024zu};
    ASSERT_NE(A.emplace<non_trivial>(&DestroyCount), nullptr);

    auto B = arena{1024zu};
    ASSERT_NE(B.emplace<non_trivial>(&DestroyCount), nullptr);

    B = std::move(A);

    EXPECT_EQ(DestroyCount, 1);

    B.reset();

    EXPECT_EQ(DestroyCount, 2);
}

TEST(arena, allocate_bytes)
{
    using namespace lambda::memory;

    auto Arena = arena{1024zu};

    auto* const Memory = Arena.allocate_bytes(64zu, alignof(std::max_align_t));
    EXPECT_NE(Memory, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(Memory) % alignof(std::max_align_t), 0);
    
    EXPECT_GE(Arena.used(), 64zu);
    EXPECT_EQ(Arena.remaining(), Arena.capacity() - Arena.used());

    auto const OldUsed = Arena.used();
    auto const OldRemaining = Arena.remaining();

    auto* const ZeroMemory = Arena.allocate_bytes(0zu, alignof(std::max_align_t));
    EXPECT_EQ(ZeroMemory, nullptr);

    EXPECT_EQ(OldUsed, Arena.used());
    EXPECT_EQ(OldRemaining, Arena.remaining());

    auto* const FatMemory = Arena.allocate_bytes(2048zu, alignof(std::max_align_t));
    EXPECT_EQ(FatMemory, nullptr);

    EXPECT_EQ(OldUsed, Arena.used());
    EXPECT_EQ(OldRemaining, Arena.remaining());
}

TEST(arena, allocate)
{
    using namespace lambda::memory;

    auto Arena = arena{1024zu};

    auto const EmptyValues = Arena.allocate<int>(0zu);
    
    EXPECT_TRUE(EmptyValues.empty());
    EXPECT_EQ(EmptyValues.data(), nullptr);
    EXPECT_EQ(Arena.used(), 0zu);

    auto const Values = Arena.allocate<int>(4);

    EXPECT_NE(Values.data(), nullptr);
    
    ASSERT_EQ(Values.size(), 4zu);
    EXPECT_EQ(Values[0], 0zu);
    EXPECT_EQ(Values[1], 0zu);
    EXPECT_EQ(Values[2], 0zu);
    EXPECT_EQ(Values[3], 0zu);

    auto const OldUsed = Arena.used();
    auto const OldRemaining = Arena.remaining();

    auto const FatValues = Arena.allocate<std::uint64_t>(1000u);

    EXPECT_TRUE(FatValues.empty());
    EXPECT_EQ(FatValues.data(), nullptr);

    EXPECT_EQ(OldUsed, Arena.used());
    EXPECT_EQ(OldRemaining, Arena.remaining());
}

TEST(arena, emplace)
{
    using namespace lambda::memory;

    auto SmallArena = arena{1zu};
    auto* const FatValue = SmallArena.emplace<std::uint64_t>(100u);

    EXPECT_EQ(FatValue, nullptr);
    EXPECT_EQ(SmallArena.used(), 0zu);

    auto Arena = arena{1024zu};
    auto* const Value = Arena.emplace<int>(42);

    ASSERT_NE(Value, nullptr);
    EXPECT_EQ(*Value, 42);

    auto const OldUsed = Arena.used();
    EXPECT_THROW({[[maybe_unused]] auto Result = Arena.emplace<throwing_constructor>(); }, int);
    EXPECT_EQ(OldUsed, Arena.used());
}

TEST(arena, reset)
{
    using namespace lambda::memory;

    auto DestroyCount = 0zu;
    auto Arena = arena{1024zu};

    ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);
    ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);

    EXPECT_EQ(DestroyCount, 0zu);

    Arena.reset();

    EXPECT_EQ(DestroyCount, 2zu);
    EXPECT_EQ(Arena.used(), 0zu);
    EXPECT_EQ(Arena.remaining(), Arena.capacity());
}

TEST(arena, scope)
{
    using namespace lambda::memory;

    auto DestroyCount = 0zu;
    auto Arena = arena{1024zu};

    {
        auto Outer = arena::scope{Arena};
        ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);
        {
            auto Inner = arena::scope{Arena};
            ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);
            ASSERT_NE(Arena.emplace<non_trivial>(&DestroyCount), nullptr);
        }
        EXPECT_EQ(DestroyCount, 2zu);
    }

    EXPECT_EQ(DestroyCount, 3zu);
    EXPECT_EQ(Arena.used(), 0zu);
}