#include "unity.h"
#include "string_lib.h"

TEST_CASE("Test string_Upper", "[string_lib]")
{
    char test_str[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    string_Upper(test_str);
    TEST_ASSERT_EQUAL_STRING("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", test_str);
}

TEST_CASE("Test string_Lower", "[string_lib]")
{
    char test_str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string_Lower(test_str);
    TEST_ASSERT_EQUAL_STRING("abcdefghijklmnopqrstuvwxyz0123456789", test_str);
}

TEST_CASE("Test string_Mirror", "[string_lib]")
{
    char test_str1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char test_str2[] = "9876543210zyxwvutsrqponmlkjihgfedcba";
    string_Mirror(test_str1);
    string_Mirror(test_str2);
    TEST_ASSERT_EQUAL_STRING("9876543210ZYXWVUTSRQPONMLKJIHGFEDCBA", test_str1);
    TEST_ASSERT_EQUAL_STRING("abcdefghijklmnopqrstuvwxyz0123456789", test_str2);
}
