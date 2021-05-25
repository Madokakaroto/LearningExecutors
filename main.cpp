#include <iostream>
#include "Forward.hpp"
#include "Concepts.hpp"
#include "Executors.hpp"

struct test_sender_traits : std::execution::sender_base
{
    template <template <typename...> class Tuple, template <typename...> class Variant>
    using value_types = Variant<Tuple<>>;

    template <template <typename...> class Variant>
    using error_types = Variant<std::exception_ptr>;

    static constexpr bool sends_done = true;
};

struct test_set_value_both
{
    friend void set_value(test_set_value_both& v, int i) noexcept
    {
        v.value_ = i + 1;
    }

    void set_value(int i) noexcept
    {
        this->value_ = i;
    }

    int value_{ 0 };
};

struct test_set_value_custom
{
    friend void set_value(test_set_value_custom& v, int i) noexcept
    {
        v.value_ = i;
    }

    int value_{ 0 };
};

struct test_set_value_default
{
    void set_value(int i) noexcept
    {
        this->value_ = i;
    }

    int value_{ 0 };
};

namespace test_adl
{
    struct foo {};

    void bar(foo)
    {
        //std::cout << "found bar" << std::endl;
    }

    struct fee_t
    {
        void operator() (foo)
        {
            //std::cout << "found fee" << std::endl;
        }
    };

    inline constexpr fee_t fee{};
}

namespace exec
{
    //template <typename R, typename ... Args> requires
    //    requires(R&& r, Args&& ... args) { std::forward<R>(r).set_value(std::forward<Args>(args)...); }
    //inline decltype(auto) set_value(R&& r, Args&& ... args)
    //noexcept(noexcept(std::declval<R>().set_value(std::declval<Args>()...)))
    //{
    //    return std::forward<R>(r).set_value(std::forward<Args>(args)...);
    //}

    //template <typename R, typename ... Args>
    //inline auto set_value(R&& r, Args&& ... args)
    //    noexcept(noexcept(set_value(std::declval<R>(), std::declval<Args>()...))) ->
    //    decltype(set_value(std::declval<R>(), std::declval<Args>()...))
    //{
    //    return set_value(std::forward<R>(r), std::forward<Args>(args)...);
    //}

    //template <typename R, typename ... Args>
    //inline void set_value(R&& r, Args&& ... args)
    //    //noexcept(noexcept(set_value(std::declval<R>(), std::declval<Args>()...)))
    //{
    //    set_value(std::forward<R>(r), std::forward<Args>(args)...);
    //}
    namespace impl
    {
        struct set_value_func
        {
            template <typename R, typename ... Args>
            void operator() (R&& r, Args&& ... args) const
            {
                set_value(std::forward<R>(r), std::forward<Args>(args)...);
            };
        };
    }
    inline constexpr impl::set_value_func set_value{};
}

int main(void)
{
    static_assert(std::execution::sender_traits<test_sender_traits>::sends_done);

    test_set_value_both both_v{};
    test_set_value_custom custom_v{};
    test_set_value_default default_v{};

    test_adl::foo f{};
    bar(f);
    //fee(f);

    exec::set_value(custom_v, 1);
    //exec::set_value(1, 1);

    return 1;
}