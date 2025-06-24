#include "unity.h"
#include "relay.h"

/* Local variables */
static Relay test_relay;
static int pin = 21;

TEST_CASE("Test relay_init", "[relay]")
{
    relay_init(&test_relay, pin);

    TEST_ASSERT_EQUAL(pin, test_relay.pin);
    TEST_ASSERT_EQUAL(0, test_relay.state);
}

TEST_CASE("Test relay_turn_on", "[relay]")
{
    relay_turn_on(&test_relay);

    TEST_ASSERT_EQUAL(1, test_relay.state);
}

TEST_CASE("Test relay_turn_off", "[relay]")
{
    relay_turn_off(&test_relay);

    TEST_ASSERT_EQUAL(0, test_relay.state);
}

TEST_CASE("Test relay_get_status", "[relay]")
{
    relay_turn_on(&test_relay);
    TEST_ASSERT_EQUAL(1, relay_get_status(&test_relay));

    relay_turn_off(&test_relay);
    TEST_ASSERT_EQUAL(0, relay_get_status(&test_relay));
}
