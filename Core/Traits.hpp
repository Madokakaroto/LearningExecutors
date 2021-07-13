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
    // has value types
    template<template
    <
        template <typename...> class Tuple,
        template <typename ...> class Variant
    > class>
    struct has_value_types;

    // has error types
    template <template <template <typename...> class> class>
    struct has_error_types;

    // get value types
    template <typename ValueTypes>
    struct get_value_types;

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

    // get error types
    template <typename ErrorTypes>
    struct get_error_types;

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

    template <typename S>
    struct sender_traits;

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
}