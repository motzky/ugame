#pragma once

#include <tuple>

#include "camera.h"
#include "math/vector3.h"

namespace game
{
    template <class T>
    struct State
    {
        T state;
    };

    struct TransformerResult
    {
        Vector3 result;
        bool stop = false;
    };

    template <class... T>
    struct ChainBase
    {
        constexpr virtual auto go(const Vector3 &, const State<T...> &) const -> Vector3
        {
            return {};
        }
    };

    template <class S, auto... T>
    struct Chain final : ChainBase<S>
    {
    };

    template <auto Head, auto... Tail, class S>
    struct Chain<S, Head, Tail...> final : ChainBase<S>
    {
        constexpr auto go(const Vector3 &in, const State<S> &state) const -> Vector3 override
        {
            const auto [result, stop] = Head(in, state);
            return stop ? result : Chain<S, Tail...>{}.go(result, state);
        }
    };

    template <auto Head, class S>
    struct Chain<S, Head> final : ChainBase<S>
    {
        constexpr auto go(const Vector3 &in, const State<S> &state) const -> Vector3 override
        {
            return Head(in, state).result;
        }
    };

    template <class S>
    struct Chain<S> final : ChainBase<S>
    {
        constexpr auto go(const Vector3 &in, const State<S> &) const -> Vector3 override
        {
            return in;
        }
    };
}