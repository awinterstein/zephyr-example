/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include "os/kernel.hpp"

ZTEST_SUITE(os_tests, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief This is just an example test for the integration of unit tests.
 */
ZTEST(os_tests, test_result_to_error_code_conversion)
{
	/* zero and positive values are not errors */
	zassert_false(os::result_to_error_code(0).operator bool());
	zassert_false(os::result_to_error_code(1).operator bool());

	/* negative values are errors */
	zassert_true(os::result_to_error_code(-1).operator bool());
	zassert_true(os::result_to_error_code(-EACCES).operator bool());

	/* resulting error codes can be compared to std::errc values */
	zassert_equal(os::result_to_error_code(-EADDRINUSE),
		      util::error_code{util::errc::address_in_use});
	zassert_equal(os::result_to_error_code(-EAGAIN),
		      util::error_code{util::errc::resource_unavailable_try_again});
}
