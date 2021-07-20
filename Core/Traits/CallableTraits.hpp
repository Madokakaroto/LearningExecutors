#pragma once

namespace std::execution
{
    template <typename Func>
    struct remove_function_noexcept;
    template <typename Ret, typename ... Args>
    struct remove_function_noexcept<Ret(*)(Args...)>
    {
        using type = Ret(*)(Args...);
        static constexpr bool value = false;
    };
    template <typename Ret, typename ... Args>
    struct remove_function_noexcept<Ret(*)(Args...) noexcept>
    {
        using type = Ret(*)(Args...);
        static constexpr bool value = true;
    };
    template <typename Func>
    using remove_function_noexceptT = typename remove_function_noexcept<Func>::Type;

    template <typename Ret, typename arguments_type, bool Noexcept>
    struct make_function;
    template <typename Ret, typename ... Args, bool Noexcept>
    struct make_function<Ret, std::tuple<Args...>, Noexcept>
    {
        using type = std::conditional_t<Noexcept, Ret(*)(Args...) noexcept, Ret(*)(Args...)>;
    };
    template <typename Ret, typename arguments_type, bool Noexcept>
    using make_function_t = typename make_function<Ret, arguments_type, Noexcept>::type;

    namespace detail
    {
        template <typename Func>
        struct function_traits;
        template <typename Ret, typename ... Args>
        struct function_traits<Ret(*)(Args...)>
        {
            using result_type = Ret;
            using signature_type = Ret(*)(Args...);
            using arguments_type = std::tuple<Args...>;

            static constexpr size_t arity = sizeof...(Args);
            static constexpr bool is_noexcept = false;
        };
        template <typename Ret, typename ... Args>
        struct function_traits<Ret(*)(Args...) noexcept> : function_traits<Ret(*)(Args...)>
        {
            using signature_type = Ret(*)(Args...) noexcept;
            static constexpr bool is_noexcept = true;
        };

        template <typename PMF>
        struct pmf_traits;
        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...)>
        {
            using result_type = Ret;
            using signature_type = Ret(T::*)(Args...);
            using arguments_type = std::tuple<Args...>;
            using caller_type = T;

            static constexpr size_t arity = sizeof...(Args);
            static constexpr bool is_const = false;
            static constexpr bool is_noexcept = false;
            static constexpr bool is_lvalue_ref_qualified = false;
            static constexpr bool is_rvalue_ref_qualified = false;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) const> : pmf_traits<Ret(T::*)(Args...)>
        {
            using signature_type = Ret(T::*)(Args...) const;
            using caller_type = T const;
            static constexpr bool is_const = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) noexcept> : pmf_traits<Ret(T::*)(Args...)>
        {
            using signature_type = Ret(T::*)(Args...) noexcept;
            static constexpr bool is_noexcept = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) &> : pmf_traits<Ret(T::*)(Args...)>
        {
            using signature_type = Ret(T::*)(Args...) &;
            static constexpr bool is_lvalue_ref_qualified = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) &&> : pmf_traits<Ret(T::*)(Args...)>
        {
            using signature_type = Ret(T::*)(Args...) &&;
            static constexpr bool is_rvalue_ref_qualified = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) const noexcept> : pmf_traits<Ret(T::*)(Args...) const>
        {
            using signature_type = Ret(T::*)(Args...) const noexcept;
            static constexpr bool is_noexcept = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) const &> : pmf_traits<Ret(T::*)(Args...) const>
        {
            using signature_type = Ret(T::*)(Args...) const &;
            static constexpr bool is_lvalue_ref_qualified = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) const &&> : pmf_traits<Ret(T::*)(Args...) const>
        {
            using signature_type = Ret(T::*)(Args...) const &&;
            static constexpr bool is_rvalue_ref_qualified = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) & noexcept> : pmf_traits<Ret(T::*)(Args...) &>
        {
            using signature_type = Ret(T::*)(Args...) & noexcept;
            static constexpr bool is_noexcept = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) && noexcept> : pmf_traits<Ret(T::*)(Args...) &&>
        {
            using signature_type = Ret(T::*)(Args...) && noexcept;
            static constexpr bool is_noexcept = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) const & noexcept> : pmf_traits<Ret(T::*)(Args...) const &&>
        {
            using signature_type = Ret(T::*)(Args...) const & noexcept;
            static constexpr bool is_noexcept = true;
        };

        template <typename T, typename Ret, typename ... Args>
        struct pmf_traits<Ret(T::*)(Args...) const && noexcept> : pmf_traits<Ret(T::*)(Args...) const &&>
        {
            using signature_type = Ret(T::*)(Args...) const && noexcept;
            static constexpr bool is_noexcept = true;
        };
    }

    // generics
    template <typename Func, typename = void>
    struct callable_traits;

    template <typename Func>
    struct callable_traits<Func, enable_if_t<is_function_v<remove_pointer_t<Func>>>>
    {
        using traits_type = detail::function_traits<std::decay_t<Func>>;
        using result_type = typename traits_type::result_type;
        using signature_type = typename traits_type::signature_type;
        using arguments_type = typename traits_type::arguments_type;
        using caller_type = void;

        static constexpr size_t arity = traits_type::arity;
        static constexpr bool is_function = true;
        static constexpr bool is_functor = false;
        static constexpr bool is_noexcept = traits_type::is_noexcept;
        static constexpr bool is_pmf = false;
        static constexpr bool is_const = true;
        static constexpr bool is_lvalue_ref_qualified = false;
        static constexpr bool is_rvalue_ref_qualified = false;
    };

    template <typename Func>
    struct callable_traits<Func, std::enable_if_t<is_functor_v<Func>>>
    {
        using traits_type = detail::pmf_traits<decltype(&Func::operator())>;
        using result_type = typename traits_type::result_type;
        using arguments_type = typename traits_type::arguments_type;
        using signature_type = make_function_t<result_type, arguments_type, traits_type::is_noexcept>;
        using caller_type = void;

        static constexpr size_t arity = traits_type::arity;
        static constexpr bool is_function = false;
        static constexpr bool is_functor = true;
        static constexpr bool is_noexcept = traits_type::is_noexcept;
        static constexpr bool is_pmf = false;
        static constexpr bool is_const = traits_type::is_const;
        static constexpr bool is_lvalue_ref_qualified = traits_type::is_lvalue_ref_qualified;
        static constexpr bool is_rvalue_ref_qualified = traits_type::is_rvalue_ref_qualified;
    };

    template <typename Func>
    struct callable_traits<Func, std::enable_if_t<std::is_member_function_pointer_v<Func>>>
    {
        using traits_type = detail::pmf_traits<Func>;
        using result_type = typename traits_type::result_type;
        using arguments_type = typename traits_type::arguments_type;
        using signature_type = typename traits_type::signature_type;
        using caller_type = typename traits_type::caller_type;

        static constexpr size_t arity = traits_type::arity;
        static constexpr bool is_function = false;
        static constexpr bool is_functor = false;
        static constexpr bool is_noexcept = traits_type::is_noexcept;
        static constexpr bool is_pmf = true;
        static constexpr bool is_const = traits_type::is_const;
        static constexpr bool is_lvalue_ref_qualified = traits_type::is_lvalue_ref_qualified;
        static constexpr bool is_rvalue_ref_qualified = traits_type::is_rvalue_ref_qualified;
    };

    template <typename Callable>
    using is_callable = disjunction<
        is_function<Callable>,
        is_member_function_pointer<Callable>,
        is_functor<Callable>>;

    template <typename Callable>
    inline constexpr bool is_callable_v = is_callable<Callable>::value;

    namespace detail
    {
        template <typename SignatureT, typename SignatureP>
        using is_same_signature_strict_impl = std::is_same<SignatureT, SignatureP>;

        template <typename SignatureT, typename SignatureP>
        using is_same_signature_impl = is_same_signature_strict_impl<
            remove_function_noexceptT<SignatureT>,
            remove_function_noexceptT<SignatureP>
        >;
    }

    template <typename FuncT, typename FuncP>
    using is_same_signature_strict = detail::is_same_signature_strict_impl<
        typename callable_traits<FuncT>::signature_type,
        typename callable_traits<FuncP>::signature_type>;
    template <typename FuncT, typename FuncP>
    inline constexpr bool is_same_signature_strict_v = is_same_signature_strict<FuncT, FuncP>::value;

    template <typename FuncT, typename FuncP>
    using is_same_signature = detail::is_same_signature_impl<
    typename callable_traits<FuncT>::signature_type,
    typename callable_traits<FuncP>::signature_type
    >;
    template <typename FuncT, typename FuncP>
    inline constexpr bool is_same_signature_v = is_same_signature<FuncT, FuncP>::value;


    template <typename Func>
    using function_decay_t = decay_t<remove_cvref_t<Func>>;
}