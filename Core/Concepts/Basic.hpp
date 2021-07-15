#pragma once

namespace std::execution
{
    template <typename S>
    concept has_sender_types = requires
    {
        typename has_value_types<S::template value_types>;
        typename has_error_types<S::template error_types>;
        typename bool_constant<S::sends_done>;
    };

    template <typename S>
    concept is_sender_base = is_base_of_v<sender_base, S>;
}