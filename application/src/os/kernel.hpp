#ifndef OS_KERNEL_HPP
#define OS_KERNEL_HPP

#include <zephyr/logging/log.h>
#include "util/system_error.hpp"
#include <chrono>

using namespace std::chrono_literals; // make the chrono literals available for all users

namespace os
{

/**
 * @brief Helper function to convert the result of a Zephyr function to an error_code.
 *
 * It takes into account that errors are returned with negative numbers and hence, counts positive
 * values as "no error".
 *
 * @param error The Zephyr error code to be converted.
 * @return Error code instance.
 */
inline util::error_code result_to_error_code(int error)
{
	// zero or positive values indicate no error
	if (error >= 0) {
		return {};
	}

	// create the error code with the generic category (errno)
	return util::make_error_code(std::errc{-error});
}

/**
 * @brief Wrapper around the millisecond sleep function of Zephyr to enable the use of std::chrono.
 */
inline std::chrono::milliseconds sleep(std::chrono::milliseconds timeout)
{
	return std::chrono::milliseconds{k_msleep(timeout.count())};
}

} // namespace os

#endif /* OS_KERNEL_HPP */
