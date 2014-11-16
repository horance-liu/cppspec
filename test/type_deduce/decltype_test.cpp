#include <gtest/gtest.h>
#include <base/assertion.h>

TEST(decltype, returns_the_type_of_that_variable_including_top_level_const_and_references)
{
    const auto ci = 10, &cr = ci;
  
    decltype(ci) a = ci;
    decltype(cr) b = ci;

    ASSERT_TYPE(const int,  a);
    ASSERT_TYPE(const int&, b);
}

TEST(decltype, with_double_parentheses_is_always_reference_type)
{
    auto i = 10;
    const auto ci = 20;

    decltype((i))  a = i;
    decltype((ci)) b = ci;
    
    ASSERT_TYPE(int&, a);
    ASSERT_TYPE(const int&, b);
}
