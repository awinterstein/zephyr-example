// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// This file is based on libcxx/include/__system_error/system_error.h
// (version: git commit f76d9d). The licence of the original file applies:
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef SYSTEM_ERROR_ERROR_CONDITION_HPP
#define SYSTEM_ERROR_ERROR_CONDITION_HPP

#include "errc.hpp"
#include "error_category.hpp"
#include <type_traits>

namespace util
{

template <class Enum>
struct is_error_condition_enum : public std::false_type {
};

template <class Enum>
inline constexpr bool is_error_condition_enum_v = is_error_condition_enum<Enum>::value;

template <>
struct is_error_condition_enum<errc> : std::true_type {
};

namespace adl_only
{
// Those cause ADL to trigger but they are not viable candidates,
// so they are never actually selected.
void make_error_condition() = delete;
} // namespace adl_only

class error_condition
{
	int value_;
	const error_category *category_;

public:
	error_condition() noexcept : value_(0), category_(&generic_category())
	{
	}

	error_condition(int value, const error_category &category) noexcept
		: value_(value), category_(&category)
	{
	}

	template <class Enum, typename = typename std::enable_if_t<is_error_condition_enum_v<Enum>>>
	error_condition(Enum enum_value) noexcept
	{
		using adl_only::make_error_condition;
		*this = make_error_condition(enum_value);
	}

	void assign(int value, const error_category &category) noexcept
	{
		value_ = value;
		category_ = &category;
	}

	template <class Enum, typename = typename std::enable_if_t<is_error_condition_enum_v<Enum>>>
	error_condition &operator=(Enum enum_value) noexcept
	{
		using adl_only::make_error_condition;
		*this = make_error_condition(enum_value);
		return *this;
	}

	void clear() noexcept
	{
		value_ = 0;
		category_ = &generic_category();
	}

	int value() const noexcept
	{
		return value_;
	}

	const error_category &category() const noexcept
	{
		return *category_;
	}
	const char *message() const;

	explicit operator bool() const noexcept
	{
		return value_ != 0;
	}
};

inline error_condition make_error_condition(errc e) noexcept
{
	return error_condition(static_cast<int>(e), generic_category());
}

inline bool operator==(const error_condition &x, const error_condition &y) noexcept
{
	return x.category() == y.category() && x.value() == y.value();
}

inline bool operator!=(const error_condition &x, const error_condition &y) noexcept
{
	return !(x == y);
}

inline bool operator<(const error_condition &x, const error_condition &y) noexcept
{
	return x.category() < y.category() ||
	       (x.category() == y.category() && x.value() < y.value());
}

} // namespace util

#endif /* SYSTEM_ERROR_ERROR_CONDITION_HPP */
