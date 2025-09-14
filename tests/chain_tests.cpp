#include <gtest/gtest.h>

#include "game/chain.h"
#include "math/vector3.h"

struct EmptyState
{
};

template <auto... T>
using TestChain = game::Chain<EmptyState, T...>;

constexpr auto AddTransformer = [](const game::Vector3 &in, const EmptyState &) -> game::TransformerResult
{ return {in + game::Vector3{1.f}}; };

constexpr auto MultiplyTransformer = [](const game::Vector3 &in, const EmptyState &) -> game::TransformerResult
{ return {in * game::Vector3{2.f}}; };

constexpr auto ChoiceTransformer = [](const game::Vector3 &in, const EmptyState &) -> game::TransformerResult
{ return {in, in.x == 1.0f}; };

TEST(transform, simple)
{
    auto state = EmptyState{};
    const auto chain = TestChain<AddTransformer>{};

    ASSERT_EQ(chain.go({}, state), game::Vector3{1.f});
}

TEST(transform, two_times_simple)
{
    auto state = EmptyState{};
    const auto chain = TestChain<AddTransformer, AddTransformer>{};

    ASSERT_EQ(chain.go({}, state), game::Vector3{2.f});
}

TEST(transform, order)
{
    auto state = EmptyState{};
    const auto chain1 = TestChain<AddTransformer, MultiplyTransformer>{};

    ASSERT_EQ(chain1.go({}, state), game::Vector3{2.f});

    const auto chain2 = TestChain<MultiplyTransformer, AddTransformer>{};

    ASSERT_EQ(chain2.go({}, state), game::Vector3{1.f});
}

TEST(transform, choice)
{
    auto state = EmptyState{};
    const auto chain1 = TestChain<AddTransformer, ChoiceTransformer, MultiplyTransformer>{};

    ASSERT_EQ(chain1.go({}, state), game::Vector3{1.f});

    const auto chain2 = TestChain<MultiplyTransformer, ChoiceTransformer, AddTransformer, AddTransformer>{};

    ASSERT_EQ(chain2.go({}, state), game::Vector3{2.f});
}
