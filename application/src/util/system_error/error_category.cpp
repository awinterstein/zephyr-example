//===----------------------------------------------------------------------===//
//
// This file is based on libcxx/src/error_category.cpp
// (version: git commit f76d9d). The licence of the original file applies:
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "error_category.hpp"
#include "error_code.hpp"

namespace util
{

// class error_category

error_condition error_category::default_error_condition(int ev) const noexcept
{
	return error_condition(ev, *this);
}

bool error_category::equivalent(int code, const error_condition &condition) const noexcept
{
	return default_error_condition(code) == condition;
}

bool error_category::equivalent(const error_code &code, int condition) const noexcept
{
	return *this == code.category() && code.value() == condition;
}

} // namespace util
