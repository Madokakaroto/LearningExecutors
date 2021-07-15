#pragma once

// basic
namespace std::execution
{
    template <typename T, template <typename ...> class Tmpl>
    struct is_instance_of : false_type {};
    template <template <typename ...> class Tmpl, typename ... Args>
    struct is_instance_of<Tmpl<Args...>, Tmpl> : true_type {};
    template <typename T, template <typename ...> class Tmpl>
    inline constexpr bool is_instance_of_v = is_instance_of<T, Tmpl>::value;

    template <typename T, typename = void>
    struct is_complete_type : std::false_type{};
    template <typename T>
    struct is_complete_type<T, std::void_t<decltype(sizeof(T))>> : std::true_type{};
    template <typename T>
    inline constexpr bool is_complete_type_v = is_complete_type<T>::value;
}

// execution
namespace std::execution
{
    // get value types impl
    template
    <
        template <typename ...> class Variant,
        template <typename ...> class Tuple,
        typename Arg
    >
    struct get_value_types<Variant<Tuple<Arg>>> { using type = Arg; };

    template
    <
        template <typename ...> class Variant,
        template <typename ...> class Tuple
    >
    struct get_value_types<Variant<Tuple<>>> { using type = void; };

    template
    <
        template <typename ...> class Variant,
        template <typename ...> class Tuple,
        typename ... Args
    > requires((sizeof...(Args) > 1))
    struct get_value_types<Variant<Tuple<Args...>>>
    {
        using type = Tuple<Args...>;
    };

    template <typename ValueTypes>
    using get_value_types_t = typename get_value_types<ValueTypes>::type;
    // end of get value types

    // get error types impl
    template
    <
        template <typename ...> class Variant,
        typename Error
    >
    struct get_error_types<Variant<Error>>
    {
        using type = Error;
    };

    template <typename ErrorTypes>
    using get_error_types_t = typename get_error_types<ErrorTypes>::type;
    // end of get error types

    template <typename T, typename Prop, typename = void>
    struct is_applicable_property : false_type {};
    template <typename T, typename Prop>
    struct is_applicable_property<T, Prop, std::enable_if_t<
        Prop::template is_applicable_property_v<T>
    >> : std::true_type {};
    template <typename T, typename Prop>
    inline constexpr bool is_applicable_property_v = is_applicable_property<T, Prop>::value;

    template <typename Prop, typename = void>
    struct is_requirable_concept : false_type {};
    template <typename Prop>
    struct is_requirable_concept<Prop, std::enable_if_t<
        Prop::is_requirable_concept
    >> : std::true_type {};
    template <typename Prop>
    inline constexpr bool is_requirable_concept_v = is_requirable_concept<Prop>::value;

    template <typename S, typename R>
    struct is_connect_invocable : false_type {};
    template <typename S, typename R>
    inline constexpr bool is_connect_invocable_v = is_connect_invocable<S, R>::value;

    template <typename E, typename F>
    struct is_execute_invocable : false_type {};
    template <typename E, typename F>
    inline constexpr bool is_execute_invocable_v = is_execute_invocable<E, F>::value;

    template <typename S, typename = void>
    struct is_invalid_sender_traits : false_type{};
    template <typename S>
    struct is_invalid_sender_traits<S, typename sender_traits<S>::__unspecialized> : true_type {};
    template <typename S>
    inline constexpr bool is_invalid_sender_traits_v = is_invalid_sender_traits<S>::value;

    template <typename O>
    struct is_start_invocable : false_type{};
    template <typename O>
    inline constexpr bool is_start_invocable_v = is_start_invocable<O>::value;

    template <typename E>
    struct is_schedule_invocable : false_type{};
    template <typename E>
    inline constexpr bool is_schedule_invocable_v = is_schedule_invocable<E>::value;
}