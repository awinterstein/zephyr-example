#include "storage/non_volatile_storage.hpp"
#include <zephyr/ztest.h>

/**
 * @brief Assert that @a error_code indicates an error (failure)
 * @param error_code std::error_code to check
 * @param ... Optional message and variables to print if the assertion fails
 */
#define zassert_error(error_code, ...)                                                             \
	zassert_true(static_cast<bool>(error_code), #error_code " is error", ##__VA_ARGS__)

/**
 * @brief Assert that @a error_code does not indicate an error (failure)
 * @param error_code std::error_code to check
 * @param ... Optional message and variables to print if the assertion fails
 */
#define zassert_no_error(error_code, ...)                                                          \
	zassert_false(static_cast<bool>(error_code), #error_code " is error", ##__VA_ARGS__)

ZTEST_SUITE(non_volatile_storage, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test writing and reading of values to and from the non-volatile storage.
 *
 * The storage gets initialized and then a value is read from the storage. As no value was written
 * before, the read operation should fail. Then a value is written to the storage and read back
 * again. The read value should be the same as the written value. Finally, the storage is cleared
 * and the read operation should fail again.
 */
ZTEST(non_volatile_storage, test_writing_and_reading)
{
	// create storage instance and initialize it
	non_volatile_storage storage{};
	zassert_no_error(storage.init());

	{ // trying to read a value before any value was written, leads to an error
		const auto value = storage.read<uint16_t>(1);
		zassert_false(value.has_value());
		zassert_error(value.error());
	}

	{ // value can be written and then retrieved successfully
		zassert_no_error(storage.write<uint16_t>(1, 42U));
		const auto value = storage.read<uint16_t>(1);
		zassert_true(value.has_value());
		zassert_equal(value.value(), 42U, "Was not able to read back the written value.");
	}

	{ // after clearing the storage, reading the value leads to an error again
		zassert_no_error(storage.clear());
		zassert_no_error(storage.init());
		const auto value = storage.read<uint16_t>(1);
		zassert_false(value.has_value());
	}
}
