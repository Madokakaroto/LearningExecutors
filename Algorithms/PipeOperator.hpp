#pragma once

namespace std::execution
{
    template <typename CPO>
    concept customise_point_object =
        copy_constructible<CPO> &&
        is_nothrow_copy_constructible_v<CPO> &&
        is_nothrow_copy_assignable_v<CPO> &&
        is_nothrow_move_constructible_v<CPO> &&
        is_nothrow_move_assignable_v<CPO>;

    template <customise_point_object CPO, move_constructible OPR>
    struct _pipe_operand_type
    {
        CPO const cpo_;
        OPR operand_;

        _pipe_operand_type(CPO cpo, OPR operand)
            : cpo_(move(cpo))
            , operand_(move(operand))
        {}
    };

    template <sender S, customise_point_object CPO, move_constructible OPR>
    inline auto operator| (S&& s, _pipe_operand_type<CPO, OPR>&& operand)
        noexcept(is_nothrow_invocable_v<CPO, S&&, OPR&&>) ->
        invoke_result_t<CPO const, S&&, OPR&&>
    {
        return operand.cpo_(forward<S>(s), move(operand.operand_));
    }

    template <sender S1, sender S2>
    inline auto operator| (S1&& s1, S2&& s2) noexcept
    {
        return forward_as_tuple(forward<S1>(s1), forward<S2>(s2));
    }

    template <sender S2, sender ... Ss>
    inline auto operator| (tuple<Ss&&...>&& ss, S2&& s2) noexcept
    {
        return tuple_cat(forward<tuple<Ss&&...>>(ss), forward_as_tuple(forward<S2>(s2)));
    }
}