// This file is based on the <system_error> implementation file from the
// GCC Runtime Library. Hence, the following copyright and licence
// definition from the original file applies.

// Copyright (C) 2007-2022 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef SYSTEM_ERROR_HPP
#define SYSTEM_ERROR_HPP

#include <bits/c++config.h>
#include <bits/error_constants.h>
#include <functional>
#include <iosfwd>
#include <type_traits>
#if __cplusplus > 201703L
#include <compare>
#endif

namespace util
{

/** @addtogroup diagnostics
 * @{
 */

class error_code;
class error_condition;

/// is_error_code_enum
template <typename _Tp>
struct is_error_code_enum : public std::false_type {
};

/// is_error_condition_enum
template <typename _Tp>
struct is_error_condition_enum : public std::false_type {
};

template <>
struct is_error_condition_enum<std::errc> : public std::true_type {
};

#if __cplusplus > 201402L
template <typename _Tp>
inline constexpr bool is_error_code_enum_v = is_error_code_enum<_Tp>::value;
template <typename _Tp>
inline constexpr bool is_error_condition_enum_v = is_error_condition_enum<_Tp>::value;
#endif // C++17
/// @}

/** @addtogroup diagnostics
 * @{
 */

/** Abstract base class for types defining a category of error codes.
 *
 * An error category defines a context that give meaning to the integer
 * stored in an `error_code` or `error_condition` object. For example,
 * the standard `errno` constants such a `EINVAL` and `ENOMEM` are
 * associated with the "generic" category and other OS-specific error
 * numbers are associated with the "system" category, but a user-defined
 * category might give different meanings to the same numerical values.
 *
 * @since C++11
 */
class error_category
{
public:
	constexpr error_category() noexcept = default;

	virtual ~error_category();

	error_category(const error_category &) = delete;
	error_category &operator=(const error_category &) = delete;

	virtual const char *name() const noexcept = 0;

	virtual const char *message(int) const = 0;

public:
	virtual error_condition default_error_condition(int __i) const noexcept;

	virtual bool equivalent(int __i, const error_condition &__cond) const noexcept;

	virtual bool equivalent(const error_code &__code, int __i) const noexcept;

	bool operator==(const error_category &__other) const noexcept
	{
		return this == &__other;
	}

#if __cpp_lib_three_way_comparison
	std::strong_ordering operator<=>(const error_category &__rhs) const noexcept
	{
		return std::compare_three_way()(this, &__rhs);
	}
#else
	bool operator!=(const error_category &__other) const noexcept
	{
		return this != &__other;
	}

	bool operator<(const error_category &__other) const noexcept
	{
		return std::less<const error_category *>()(this, &__other);
	}
#endif
};

// DR 890.

/// Error category for `errno` error codes.
_GLIBCXX_CONST const error_category &generic_category() noexcept;

/// Error category for other error codes defined by the OS.
_GLIBCXX_CONST const error_category &system_category() noexcept;

/// @}

/** @addtogroup diagnostics
 * @{
 */

error_code make_error_code(std::errc) noexcept;

/** Class error_code
 *
 * This class is a value type storing an integer error number and a
 * category that gives meaning to the error number. Typically this is done
 * close the the point where the error happens, to capture the original
 * error value.
 *
 * An `error_code` object can be used to store the original error value
 * emitted by some subsystem, with a category relevant to the subsystem.
 * For example, errors from POSIX library functions can be represented by
 * an `errno` value and the "generic" category, but errors from an HTTP
 * library might be represented by an HTTP response status code (e.g. 404)
 * and a custom category defined by the library.
 *
 * @since C++11
 * @ingroup diagnostics
 */
class error_code
{
public:
	error_code() noexcept : _M_value(0), _M_cat(&system_category())
	{
	}

	error_code(int __v, const error_category &__cat) noexcept : _M_value(__v), _M_cat(&__cat)
	{
	}

	template <typename _ErrorCodeEnum, typename = typename std::enable_if<
						   is_error_code_enum<_ErrorCodeEnum>::value>::type>
	error_code(_ErrorCodeEnum __e) noexcept
	{
		*this = make_error_code(__e);
	}

	void assign(int __v, const error_category &__cat) noexcept
	{
		_M_value = __v;
		_M_cat = &__cat;
	}

	void clear() noexcept
	{
		assign(0, system_category());
	}

	// DR 804.
	template <typename _ErrorCodeEnum>
	typename std::enable_if<is_error_code_enum<_ErrorCodeEnum>::value, error_code &>::type
	operator=(_ErrorCodeEnum __e) noexcept
	{
		return *this = make_error_code(__e);
	}

	int value() const noexcept
	{
		return _M_value;
	}

	const error_category &category() const noexcept
	{
		return *_M_cat;
	}

	error_condition default_error_condition() const noexcept;

	const char *message() const
	{
		return category().message(value());
	}

	explicit operator bool() const noexcept
	{
		return _M_value != 0;
	}

	// DR 804.
private:
	int _M_value;
	const error_category *_M_cat;
};

// 19.4.2.6 non-member functions

/// @relates error_code @{

inline error_code make_error_code(std::errc __e) noexcept
{
	return error_code(static_cast<int>(__e), generic_category());
}

#if __cpp_lib_three_way_comparison
inline std::strong_ordering operator<=>(const error_code &__lhs, const error_code &__rhs) noexcept
{
	if (auto __c = __lhs.category() <=> __rhs.category(); __c != 0) {
		return __c;
	}
	return __lhs.value() <=> __rhs.value();
}
#else
inline bool operator<(const error_code &__lhs, const error_code &__rhs) noexcept
{
	return (__lhs.category() < __rhs.category() ||
		(__lhs.category() == __rhs.category() && __lhs.value() < __rhs.value()));
}
#endif

template <typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits> &operator<<(std::basic_ostream<_CharT, _Traits> &__os,
						const error_code &__e)
{
	return (__os << __e.category().name() << ':' << __e.value());
}

/// @}

error_condition make_error_condition(std::errc) noexcept;

/** Class error_condition
 *
 * This class represents error conditions that may be visible at an API
 * boundary. Different `error_code` values that can occur within a library
 * or module might map to the same `error_condition`.
 *
 * An `error_condition` represents something that the program can test for,
 * and subsequently take appropriate action.
 *
 * @since C++11
 */
class error_condition
{
public:
	error_condition() noexcept : _M_value(0), _M_cat(&generic_category())
	{
	}

	error_condition(int __v, const error_category &__cat) noexcept
		: _M_value(__v), _M_cat(&__cat)
	{
	}

	template <typename _ErrorConditionEnum,
		  typename = typename std::enable_if<
			  is_error_condition_enum<_ErrorConditionEnum>::value>::type>
	error_condition(_ErrorConditionEnum __e) noexcept
	{
		*this = make_error_condition(__e);
	}

	void assign(int __v, const error_category &__cat) noexcept
	{
		_M_value = __v;
		_M_cat = &__cat;
	}

	// DR 804.
	template <typename _ErrorConditionEnum>
	typename std::enable_if<is_error_condition_enum<_ErrorConditionEnum>::value,
				error_condition &>::type
	operator=(_ErrorConditionEnum __e) noexcept
	{
		return *this = make_error_condition(__e);
	}

	void clear() noexcept
	{
		assign(0, generic_category());
	}

	// 19.4.3.4 observers
	int value() const noexcept
	{
		return _M_value;
	}

	const error_category &category() const noexcept
	{
		return *_M_cat;
	}

	const char *message() const
	{
		return category().message(value());
	}

	explicit operator bool() const noexcept
	{
		return _M_value != 0;
	}

	// DR 804.
private:
	int _M_value;
	const error_category *_M_cat;
};

// 19.4.3.6 non-member functions

/// Create an `error_condition` representing a standard `errc` condition.
/// @relates error_condition
inline error_condition make_error_condition(std::errc __e) noexcept
{
	return error_condition(static_cast<int>(__e), generic_category());
}

// 19.4.4 Comparison operators

/// @relates error_code
inline bool operator==(const error_code &__lhs, const error_code &__rhs) noexcept
{
	return (__lhs.category() == __rhs.category() && __lhs.value() == __rhs.value());
}

/// @relates error_code
inline bool operator==(const error_code &__lhs, const error_condition &__rhs) noexcept
{
	return (__lhs.category().equivalent(__lhs.value(), __rhs) ||
		__rhs.category().equivalent(__lhs, __rhs.value()));
}

/// @relates error_condition
inline bool operator==(const error_condition &__lhs, const error_condition &__rhs) noexcept
{
	return (__lhs.category() == __rhs.category() && __lhs.value() == __rhs.value());
}

#if __cpp_lib_three_way_comparison
/// Define an ordering for error_condition objects.
/// @relates error_condition
inline std::strong_ordering operator<=>(const error_condition &__lhs,
					const error_condition &__rhs) noexcept
{
	if (auto __c = __lhs.category() <=> __rhs.category(); __c != 0) {
		return __c;
	}
	return __lhs.value() <=> __rhs.value();
}
#else
/// Define an ordering for error_condition objects.
/// @relates error_condition
inline bool operator<(const error_condition &__lhs, const error_condition &__rhs) noexcept
{
	return (__lhs.category() < __rhs.category() ||
		(__lhs.category() == __rhs.category() && __lhs.value() < __rhs.value()));
}

/// @relates error_condition
inline bool operator==(const error_condition &__lhs, const error_code &__rhs) noexcept
{
	return (__rhs.category().equivalent(__rhs.value(), __lhs) ||
		__lhs.category().equivalent(__rhs, __lhs.value()));
}

/// @relates error_code
inline bool operator!=(const error_code &__lhs, const error_code &__rhs) noexcept
{
	return !(__lhs == __rhs);
}

/// @relates error_code
inline bool operator!=(const error_code &__lhs, const error_condition &__rhs) noexcept
{
	return !(__lhs == __rhs);
}

/// @relates error_condition
inline bool operator!=(const error_condition &__lhs, const error_code &__rhs) noexcept
{
	return !(__lhs == __rhs);
}

/// @relates error_condition
inline bool operator!=(const error_condition &__lhs, const error_condition &__rhs) noexcept
{
	return !(__lhs == __rhs);
}
#endif // three_way_comparison
/// @}

} // namespace util

#include <bits/functional_hash.h>

namespace std
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#ifndef _GLIBCXX_COMPATIBILITY_CXX0X
// DR 1182.
/// std::hash specialization for error_code.
/// @relates error_code
template <>
struct hash<util::error_code> : public __hash_base<size_t, util::error_code> {
	size_t operator()(const util::error_code &__e) const noexcept
	{
		const size_t __tmp = std::_Hash_impl::hash(__e.value());
		return std::_Hash_impl::__hash_combine(&__e.category(), __tmp);
	}
};
#endif // _GLIBCXX_COMPATIBILITY_CXX0X

#if __cplusplus >= 201703L
// DR 2686.
/// std::hash specialization for error_condition.
/// @relates error_condition
template <>
struct hash<util::error_condition> : public __hash_base<size_t, util::error_condition> {
	size_t operator()(const util::error_condition &__e) const noexcept
	{
		const size_t __tmp = std::_Hash_impl::hash(__e.value());
		return std::_Hash_impl::__hash_combine(&__e.category(), __tmp);
	}
};
#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif /* SYSTEM_ERROR_HPP */
