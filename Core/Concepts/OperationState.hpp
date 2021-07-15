#pragma once

namespace std::execution
{
    template <typename O>
    concept operation_state =
        destructible<O> &&
        is_object_v<O> &&
        is_start_invocable_v<O>;
}