//===----------------------------------------------------------------------===//
//
// This file is based on libcxx/include/__system_error/error_category.h
// (version: git commit f76d9d). The licence of the original file applies:
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef SYSTEM_ERROR_ERROR_CATEGORY_HPP
#define SYSTEM_ERROR_ERROR_CATEGORY_HPP

namespace util
{
class error_condition;
class error_code;

class error_category
{
public:
	virtual ~error_category() noexcept = default;

	error_category() noexcept = default;
	error_category(const error_category &) = delete;
	error_category &operator=(const error_category &) = delete;

	virtual const char *name() const noexcept = 0;
	virtual error_condition default_error_condition(int ev) const noexcept;
	virtual bool equivalent(int code, const error_condition &condition) const noexcept;
	virtual bool equivalent(const error_code &code, int condition) const noexcept;
	virtual const char *message(int ev) const noexcept = 0;

	bool operator==(const error_category &rhs) const noexcept
	{
		return this == &rhs;
	}

	bool operator!=(const error_category &rhs) const noexcept
	{
		return !(*this == rhs);
	}

	bool operator<(const error_category &rhs) const noexcept
	{
		return this < &rhs;
	}
};

const error_category &system_category() noexcept;
const error_category &generic_category() noexcept;

} // namespace util

#endif /* SYSTEM_ERROR_ERROR_CATEGORY_HPP */
