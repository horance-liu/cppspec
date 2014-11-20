#include <gtest/gtest.h>
#include <array>
#include "assertion/type_assertion.h"

namespace
{
    class C1 {};

    struct C2
    {
        virtual void func1() = 0;
        virtual void func2(int) const = 0;
        virtual void func3() volatile = 0;
        virtual void func4() const volatile = 0;

        virtual ~C2();
    };

    struct C3 : C2
    {
        int a;
        std::vector<int> b;

    private:
        void func1() override {}
        virtual void func2(int) const override {}
        virtual void func3() volatile override {}
        virtual void func4() const volatile override {}
    };

    enum E1 { E1_ENUM };
    enum class E2 {};
    enum class E3 : int {};
    enum struct E4 {};

    union U1 { int i; long l; };
    union U2 { C1 c1; C3 c3; };
}

TEST(primary_type, is_void)
{
    STATIC_ASSERT_TRUE(std::is_void<void>::value);
    STATIC_ASSERT_TRUE(std::is_void<const void>::value);
    STATIC_ASSERT_TRUE(std::is_void<volatile void>::value);
    STATIC_ASSERT_TRUE(std::is_void<volatile const void>::value);
}

TEST(primary_type, is_integral)
{
    STATIC_ASSERT_TRUE(std::is_integral<char>::value);
    STATIC_ASSERT_TRUE(std::is_integral<short>::value);
    STATIC_ASSERT_TRUE(std::is_integral<int>::value);
    STATIC_ASSERT_TRUE(std::is_integral<long>::value);
    STATIC_ASSERT_TRUE(std::is_integral<long long>::value);
}

TEST(primary_type, is_float)
{
    STATIC_ASSERT_TRUE(std::is_floating_point<float>::value);
    STATIC_ASSERT_TRUE(std::is_floating_point<double>::value);
    STATIC_ASSERT_TRUE(std::is_floating_point<long double>::value);
}

namespace std_cpp14
{
    template <class T> struct __cpplib_is_null_pointer : std::false_type {};
    template <> struct __cpplib_is_null_pointer<std::nullptr_t> : std::true_type {};

    template <class T> struct is_null_pointer
        : __cpplib_is_null_pointer<typename std::remove_cv<T>::type> {};
}

TEST(primary_type, is_null_pointer_cpp14)
{
    STATIC_ASSERT_TRUE(std_cpp14::is_null_pointer< decltype(nullptr) >::value);
    STATIC_ASSERT_TRUE(std_cpp14::is_null_pointer< std::nullptr_t >::value);
    STATIC_ASSERT_TRUE(std_cpp14::is_null_pointer< const std::nullptr_t >::value);
    STATIC_ASSERT_TRUE(std_cpp14::is_null_pointer< volatile std::nullptr_t >::value);
    STATIC_ASSERT_TRUE(std_cpp14::is_null_pointer< const volatile std::nullptr_t >::value);
}

TEST(primary_type, is_array)
{
    STATIC_ASSERT_TRUE(std::is_array<int[]>::value);
    STATIC_ASSERT_TRUE(std::is_array<int[10]>::value);
    STATIC_ASSERT_TRUE(std::is_array<C1[]>::value);
    STATIC_ASSERT_TRUE(std::is_array<C1[10]>::value);

    using int_array = std::array<int, 10>;
    STATIC_ASSERT_FALSE(std::is_array<int_array>::value);
}

TEST(primary_type, is_enum)
{
    STATIC_ASSERT_FALSE(std::is_enum<C1>::value);
    STATIC_ASSERT_TRUE(std::is_enum<E1>::value);
    STATIC_ASSERT_TRUE(std::is_enum<E2>::value);
    STATIC_ASSERT_TRUE(std::is_enum<E3>::value);
    STATIC_ASSERT_TRUE(std::is_enum<E4>::value);

    STATIC_ASSERT_TRUE(std::is_enum<decltype(E1_ENUM)>::value);
}

TEST(primary_type, is_union)
{
    STATIC_ASSERT_TRUE(std::is_union<U1>::value);
    STATIC_ASSERT_TRUE(std::is_union<U2>::value);

    STATIC_ASSERT_FALSE(std::is_union<C1>::value);
    STATIC_ASSERT_FALSE(std::is_union<E1>::value);
}

TEST(primary_type, is_class)
{
    STATIC_ASSERT_TRUE(std::is_class<C1>::value);
    STATIC_ASSERT_TRUE(std::is_class<C2>::value);
    STATIC_ASSERT_FALSE(std::is_class<E2>::value);
}

namespace
{
    double func1(float) { return 0.0; }

    auto func2(int, int) -> double (*) (float)
    {
        return func1;
    }

    using func2_alias = auto (int, int) -> double (*) (float);
}

TEST(primary_type, is_function)
{
    STATIC_ASSERT_TRUE(std::is_function<double(float)>::value);
    STATIC_ASSERT_TRUE(std::is_function<decltype(func1)>::value);

    STATIC_ASSERT_TRUE(std::is_function<auto (int, int)->double(*)(float)>::value);
    STATIC_ASSERT_TRUE(std::is_function<decltype(func2)>::value);
    STATIC_ASSERT_TRUE(std::is_function<func2_alias>::value);

    STATIC_ASSERT_TRUE(std::is_function<int(const char*, ...)>::value);
    STATIC_ASSERT_FALSE(std::is_function<decltype(&C2::func1)>::value);
}

TEST(primary_key, is_member_function_pointer)
{
    STATIC_ASSERT_TRUE(std::is_member_function_pointer<void (C2::*)()>::value);
    STATIC_ASSERT_TRUE(std::is_member_function_pointer<void (C2::*)() const>::value);

    STATIC_ASSERT_TRUE(std::is_member_function_pointer<decltype(&C2::func1)>::value);
    STATIC_ASSERT_TRUE(std::is_member_function_pointer<decltype(&C2::func2)>::value);

    using c2_func1_type = void (C2::*)();
    STATIC_ASSERT_TRUE(std::is_member_function_pointer<c2_func1_type>::value);

    using c2_func2_type = void (C2::*)(int) const;
    STATIC_ASSERT_TRUE(std::is_member_function_pointer<c2_func1_type>::value);
}

TEST(primary_key, is_member_object_pointer)
{
    STATIC_ASSERT_TRUE(std::is_member_object_pointer<int C3::*>::value);
    STATIC_ASSERT_TRUE(std::is_member_object_pointer<std::vector<int> C3::*>::value);

    STATIC_ASSERT_TRUE(std::is_member_object_pointer<decltype(&C3::a)>::value);
    STATIC_ASSERT_TRUE(std::is_member_object_pointer<decltype(&C3::b)>::value);

    using c3_b_type = std::vector<int> C3::*;
    STATIC_ASSERT_TRUE(std::is_member_object_pointer<c3_b_type>::value);

    using c3_a_type = int C3::*;
    STATIC_ASSERT_TRUE(std::is_member_object_pointer<c3_a_type>::value);
}

TEST(primary_type, is_pointer)
{
    STATIC_ASSERT_TRUE(std::is_pointer<int*>::value);
    STATIC_ASSERT_TRUE(std::is_pointer<int**>::value);
    STATIC_ASSERT_TRUE(std::is_pointer<const int*>::value);
    STATIC_ASSERT_TRUE(std::is_pointer<C1*>::value);
    STATIC_ASSERT_TRUE(std::is_pointer<const C1*>::value);
    STATIC_ASSERT_FALSE(std::is_pointer<std::nullptr_t>::value);
}

TEST(primary_type, is_lvalue_reference)
{
    STATIC_ASSERT_TRUE(std::is_lvalue_reference<int&>::value);
    STATIC_ASSERT_TRUE(std::is_lvalue_reference<const int&>::value);
    STATIC_ASSERT_TRUE(std::is_lvalue_reference<C1&>::value);
    STATIC_ASSERT_TRUE(std::is_lvalue_reference<const C1&>::value);

    STATIC_ASSERT_FALSE(std::is_lvalue_reference<int>::value);
    STATIC_ASSERT_FALSE(std::is_lvalue_reference<int*>::value);
    STATIC_ASSERT_FALSE(std::is_lvalue_reference<int&&>::value);
    STATIC_ASSERT_FALSE(std::is_lvalue_reference<const int&&>::value);
}

TEST(primary_type, is_rvalue_reference)
{
    STATIC_ASSERT_TRUE(std::is_rvalue_reference<int&&>::value);
    STATIC_ASSERT_TRUE(std::is_rvalue_reference<const int&&>::value);
    STATIC_ASSERT_TRUE(std::is_rvalue_reference<C1&&>::value);
    STATIC_ASSERT_TRUE(std::is_rvalue_reference<const C1&&>::value);

    STATIC_ASSERT_FALSE(std::is_rvalue_reference<int>::value);
    STATIC_ASSERT_FALSE(std::is_rvalue_reference<int*>::value);
    STATIC_ASSERT_FALSE(std::is_rvalue_reference<int&>::value);
    STATIC_ASSERT_FALSE(std::is_rvalue_reference<const int&>::value);
}

TEST(composite_type, is_arithmetic)
{
    STATIC_ASSERT_TRUE(std::is_arithmetic<int>::value);
    STATIC_ASSERT_TRUE(std::is_arithmetic<float>::value);

    STATIC_ASSERT_FALSE(std::is_arithmetic<void>::value);
    STATIC_ASSERT_FALSE(std::is_arithmetic<std::nullptr_t>::value);

    STATIC_ASSERT_FALSE(std::is_arithmetic<int&>::value);
    STATIC_ASSERT_FALSE(std::is_arithmetic<float*>::value);

    STATIC_ASSERT_FALSE(std::is_arithmetic<C1>::value);
    STATIC_ASSERT_FALSE(std::is_arithmetic<E1>::value);
    STATIC_ASSERT_FALSE(std::is_arithmetic<U1>::value);
}

TEST(composite_type, is_fundamental)
{
    STATIC_ASSERT_TRUE(std::is_fundamental<void>::value);
    STATIC_ASSERT_TRUE(std::is_fundamental<int>::value);
    STATIC_ASSERT_TRUE(std::is_fundamental<float>::value);
    STATIC_ASSERT_TRUE(std::is_fundamental<std::nullptr_t>::value);

    STATIC_ASSERT_FALSE(std::is_fundamental<int&>::value);
    STATIC_ASSERT_FALSE(std::is_fundamental<float*>::value);

    STATIC_ASSERT_FALSE(std::is_fundamental<C1>::value);
    STATIC_ASSERT_FALSE(std::is_fundamental<E1>::value);
    STATIC_ASSERT_FALSE(std::is_fundamental<U1>::value);
}

TEST(composite_type, is_compound)
{
    STATIC_ASSERT_FALSE(std::is_compound<void>::value);
    STATIC_ASSERT_FALSE(std::is_compound<int>::value);
    STATIC_ASSERT_FALSE(std::is_compound<float>::value);
    STATIC_ASSERT_FALSE(std::is_compound<std::nullptr_t>::value);

    STATIC_ASSERT_TRUE(std::is_compound<int&>::value);
    STATIC_ASSERT_TRUE(std::is_compound<float*>::value);

    STATIC_ASSERT_TRUE(std::is_compound<C1>::value);
    STATIC_ASSERT_TRUE(std::is_compound<E1>::value);
    STATIC_ASSERT_TRUE(std::is_compound<U1>::value);
}

TEST(composite_type, is_reference)
{
    STATIC_ASSERT_TRUE(std::is_compound<int&>::value);
    STATIC_ASSERT_TRUE(std::is_compound<C1&>::value);

    STATIC_ASSERT_TRUE(std::is_compound<int&&>::value);
    STATIC_ASSERT_TRUE(std::is_compound<C1&&>::value);
}

TEST(composite_type, is_member_pointer)
{
    STATIC_ASSERT_TRUE(std::is_member_pointer<int C3::*>::value);
    STATIC_ASSERT_TRUE(std::is_member_pointer<void (C3::*)(int) const>::value);
}

TEST(composite_type, is_scalar)
{
    STATIC_ASSERT_TRUE(std::is_scalar<int>::value);
    STATIC_ASSERT_TRUE(std::is_scalar<float>::value);
    STATIC_ASSERT_TRUE(std::is_scalar<E1>::value);
    STATIC_ASSERT_TRUE(std::is_scalar<int*>::value);
    STATIC_ASSERT_TRUE(std::is_scalar<int C3::*>::value);
    STATIC_ASSERT_TRUE(std::is_scalar<void (C3::*) (int) const>::value);
    STATIC_ASSERT_TRUE(std::is_scalar<std::nullptr_t>::value);
}

TEST(type_property, is_const)
{
    STATIC_ASSERT_FALSE(std::is_const<int>::value);
    STATIC_ASSERT_TRUE(std::is_const<const int>::value);

    STATIC_ASSERT_FALSE(std::is_const<C2>::value);
    STATIC_ASSERT_TRUE(std::is_const<const C2>::value);

    STATIC_ASSERT_FALSE(std::is_const<const C2&>::value);
    STATIC_ASSERT_FALSE(std::is_const<const C2*>::value);

    STATIC_ASSERT_FALSE(std::is_const<decltype(E1_ENUM)>::value);
}

TEST(supported_operations, is_trivially_constructible)
{

}
