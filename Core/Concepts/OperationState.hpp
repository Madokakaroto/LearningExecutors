#pragma once

namespace std::execution
{
    template <typename O>
    concept operation_state =
        destructible<O> &&
        is_object_v<O> &&
        requires(O& o)
        {
            { execution::start(o) } noexcept;
        };
}