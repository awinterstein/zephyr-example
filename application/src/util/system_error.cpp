//===----------------------------------------------------------------------===//
//
// This file is based on libcxx/src/system_error.cpp
// (version: git commit f76d9d). The licence of the original file applies:
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "system_error.hpp"
#include <cstring>

namespace util
{

// error_category

class generic_error_category : public error_category
{
public:
	virtual const char *name() const noexcept;
	virtual const char *message(int ev) const noexcept;
};

const char *generic_error_category::name() const noexcept
{
	return "generic";
}

const char *generic_error_category::message(int ev) const noexcept
{
	return strerror(ev);
}

const generic_error_category the_generic_error_category{};

const error_category &generic_category() noexcept
{
	return the_generic_error_category;
}

class system_error_category : public error_category
{
public:
	virtual const char *name() const noexcept;
	virtual const char *message(int ev) const noexcept;
	virtual error_condition default_error_condition(int ev) const noexcept;
};

const char *system_error_category::name() const noexcept
{
	return "system";
}

const char *system_error_category::message(int ev) const noexcept
{
	return strerror(ev);
}

error_condition system_error_category::default_error_condition(int ev) const noexcept
{
	return error_condition(ev, generic_category());
}

const system_error_category the_system_error_category{};

const error_category &system_category() noexcept
{
	return the_system_error_category;
}

// error_condition

const char *error_condition::message() const
{
	return category_->message(value_);
}

// error_code

const char *error_code::message() const
{
	return category_->message(value_);
}

} // namespace util
