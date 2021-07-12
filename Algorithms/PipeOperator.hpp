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
}