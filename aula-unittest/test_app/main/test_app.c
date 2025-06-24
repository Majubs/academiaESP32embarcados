#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "unity_internals.h"
#include "unity_test_runner.h"

#define TEST_ALL 0

static void print_banner(const char *text);

void app_main(void)
{
    printf("===== TEST APPLICATION =====");
    /* These are the different ways of running registered tests.
     * In practice, only one of them is usually needed.
     *
     * UNITY_BEGIN() and UNITY_END() calls tell Unity to print a summary
     * (number of tests executed/failed/ignored) of tests executed between these calls.
     */

#if TEST_ALL == 0
    print_banner("Executing one test by its name");
    UNITY_BEGIN();
    unity_run_test_by_name("Test relay_init");
    UNITY_END();

    print_banner("Executring tests with [relay] tag");
    UNITY_BEGIN();
    unity_run_tests_by_tag("[relay]", false);
    UNITY_END();

    print_banner("Executring tests without [relay] tag");
    UNITY_BEGIN();
    unity_run_tests_by_tag("[relay]", true);
    UNITY_END();
#else
    print_banner("Running all the registered tests!");
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();
#endif

    // print_banner("Starting interactive test menu:");
    // unity_run_menu();
}

static void print_banner(const char *text) { printf("\n#### %s ####\n\n", text); }
