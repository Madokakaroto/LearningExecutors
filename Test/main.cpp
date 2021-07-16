#include <iostream>
#include <exception>
#include "../Executors.hpp"

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

inline constexpr struct _sink {
    void set_value(auto&&...) const noexcept {}
    [[noreturn]] void set_error(auto&&) const noexcept {
        std::terminate();
    }
    [[noreturn]] void set_done() const noexcept {
        std::terminate();
    }
} sink{};

template <typename T>
inline constexpr bool is_something = false;

template <typename T>
struct is_something_type
{
    using __type = void;
};

template <typename T>
concept is_something_type_concept =
    !requires
    {
        typename is_something_type<T>::__type;
    };

template <typename T> requires(std::is_integral_v<T>)
struct is_something_type<T>
{
    using type = T;
};

template <typename T>
concept is_something_concept = is_something<T>;

template <typename T> requires(std::is_integral_v<T>)
inline constexpr bool is_something<T> = true;


template <typename T, typename>
struct sfinae_concept_type
{
    using __type = void;
};

template <typename T>
concept is_sfinae_type_concept =
!requires
{
    typename is_something_type<T>::__type;
};


template <typename T>
struct sfinae_concept_type<T, std::enable_if<std::is_integral_v<T>>>
{
    using type = T;
};

void throw_function()
{
    throw 2;
}

void process_excetion(std::exception_ptr const& e)
{
    try
    {
        std::rethrow_exception(e);
    }
    catch(int i)
    {
        std::cout << "Error int:" << i << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main(void)
{
    static_assert(is_something_concept<int>);
    static_assert(is_something_type_concept<int>);
    static_assert(is_sfinae_type_concept<int>);
    static_assert(!is_sfinae_type_concept<float>);

    using namespace std::execution;

    static_assert(std::execution::sender_traits<test_sender_traits>::sends_done);

    test_set_value_both both_v{};
    test_set_value_custom custom_v{};
    test_set_value_default default_v{};

    std::execution::set_value(custom_v, 1);
    //exec::set_value(1, 1);

    auto just_sender = std::execution::just(1, 2, 3);
    auto f = [](int i, int j, int k) noexcept
    {
        printf("i=%d, j=%d, k=%d\n", i, j, k);
    };

    std::execution::start(std::execution::connect(std::execution::transform(just_sender, f), sink));


    static_thread_pool pool{ 4 };
    auto r = sync_wait(
        just(2, 3) |
        on(pool.get_scheduler()) |
        transform([](int i, int j){ return (i + j) * 0.2; }) |
        let_value([](double r){ return r > 0.1; }));


    try
    {
        throw_function();
    }
    catch(...)
    {
        process_excetion(std::current_exception());
    }

    return 1;
}