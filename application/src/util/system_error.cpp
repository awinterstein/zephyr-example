// This file is based on the <system_error> implementation file from the
// GCC Runtime Library. Hence, the following copyright and licence
// definition from the original file applies.

// Copyright (C) 2007-2024 Free Software Foundation, Inc.
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

#define _GLIBCXX_USE_CXX11_ABI 1
#define __sso_string           __sso_stringxxx
#include "system_error.hpp"
#include <errno.h>
#include <bits/functexcept.h>
#include <cstring>
#include <limits>
#undef __sso_string

#if __has_cpp_attribute(clang::require_constant_initialization)
#define __constinit [[clang::require_constant_initialization]]
#endif

namespace
{
template <typename T>
struct constant_init {
	union {
		T obj;
	};
	constexpr constant_init() : obj()
	{
	}

	~constant_init()
	{ /* do nothing, union member is not destroyed */
	}
};

struct generic_error_category final : public util::error_category {
	const char *name() const noexcept final
	{
		return "generic";
	}

	_GLIBCXX_DEFAULT_ABI_TAG
	const char *message(int i) const final
	{
		return strerror(i);
	}

	// Override this to avoid a virtual call to default_error_condition(i).
	bool equivalent(int i, const util::error_condition &cond) const noexcept final
	{
		return i == cond.value() && *this == cond.category();
	}
};

__constinit constant_init<generic_error_category> generic_category_instance{};

struct system_error_category final : public util::error_category {
	const char *name() const noexcept final
	{
		return "system";
	}

	_GLIBCXX_DEFAULT_ABI_TAG
	const char *message(int i) const final
	{
		return strerror(i);
	}

	util::error_condition default_error_condition(int ev) const noexcept final
	{
		// Use generic category for all known POSIX errno values (including zero)
		// and system category otherwise.
		switch (ev) {
#if defined __AVR__
		// avr-libc only defines a few distinct error numbers. Most <errno.h>
		// constants are not usable in #if directives and have the same value.
		case EDOM:
		case ERANGE:
		case ENOSYS:
		case EINTR:
		case 0:
			return std::error_condition(ev, generic_category_instance.obj);
#else
			// List of errno macros from [cerrno.syn].
			// C11 only defines EDOM, EILSEQ and ERANGE, the rest are from POSIX.
			// They expand to integer constant expressions with type int,
			// and distinct positive values, suitable for use in #if directives.
			// POSIX adds more macros (but they're not defined on all targets,
			// see config/os/.../error_constants.h), and POSIX allows
			// EAGAIN == EWOULDBLOCK and ENOTSUP == EOPNOTSUPP.

#ifdef E2BIG
		case E2BIG:
#endif
#ifdef EACCES
		case EACCES:
#endif
#ifdef EADDRINUSE
		case EADDRINUSE:
#endif
#ifdef EADDRNOTAVAIL
		case EADDRNOTAVAIL:
#endif
#ifdef EAFNOSUPPORT
		case EAFNOSUPPORT:
#endif
#ifdef EAGAIN
		case EAGAIN:
#endif
#ifdef EALREADY
		case EALREADY:
#endif
#ifdef EBADF
		case EBADF:
#endif
#ifdef EBADMSG
		case EBADMSG:
#endif
#ifdef EBUSY
		case EBUSY:
#endif
#ifdef ECANCELED
		case ECANCELED:
#endif
#ifdef ECHILD
		case ECHILD:
#endif
#ifdef ECONNABORTED
		case ECONNABORTED:
#endif
#ifdef ECONNREFUSED
		case ECONNREFUSED:
#endif
#ifdef ECONNRESET
		case ECONNRESET:
#endif
#ifdef EDEADLK
		case EDEADLK:
#endif
#ifdef EDESTADDRREQ
		case EDESTADDRREQ:
#endif
		case EDOM:
#ifdef EEXIST
		case EEXIST:
#endif
#ifdef EFAULT
		case EFAULT:
#endif
#ifdef EFBIG
		case EFBIG:
#endif
#ifdef EHOSTUNREACH
		case EHOSTUNREACH:
#endif
#ifdef EIDRM
		case EIDRM:
#endif
		case EILSEQ:
#ifdef EINPROGRESS
		case EINPROGRESS:
#endif
#ifdef EINTR
		case EINTR:
#endif
#ifdef EINVAL
		case EINVAL:
#endif
#ifdef EIO
		case EIO:
#endif
#ifdef EISCONN
		case EISCONN:
#endif
#ifdef EISDIR
		case EISDIR:
#endif
#ifdef ELOOP
		case ELOOP:
#endif
#ifdef EMFILE
		case EMFILE:
#endif
#ifdef EMLINK
		case EMLINK:
#endif
#ifdef EMSGSIZE
		case EMSGSIZE:
#endif
#ifdef ENAMETOOLONG
		case ENAMETOOLONG:
#endif
#ifdef ENETDOWN
		case ENETDOWN:
#endif
#ifdef ENETRESET
		case ENETRESET:
#endif
#ifdef ENETUNREACH
		case ENETUNREACH:
#endif
#ifdef ENFILE
		case ENFILE:
#endif
#ifdef ENOBUFS
		case ENOBUFS:
#endif
#ifdef ENODATA
		case ENODATA:
#endif
#ifdef ENODEV
		case ENODEV:
#endif
#ifdef ENOENT
		case ENOENT:
#endif
#ifdef ENOEXEC
		case ENOEXEC:
#endif
#ifdef ENOLCK
		case ENOLCK:
#endif
#ifdef ENOLINK
		case ENOLINK:
#endif
#ifdef ENOMEM
		case ENOMEM:
#endif
#ifdef ENOMSG
		case ENOMSG:
#endif
#ifdef ENOPROTOOPT
		case ENOPROTOOPT:
#endif
#ifdef ENOSPC
		case ENOSPC:
#endif
#ifdef ENOSR
		case ENOSR:
#endif
#ifdef ENOSTR
		case ENOSTR:
#endif
#ifdef ENOSYS
		case ENOSYS:
#endif
#ifdef ENOTCONN
		case ENOTCONN:
#endif
#ifdef ENOTDIR
		case ENOTDIR:
#endif
#if defined ENOTEMPTY && (!defined EEXIST || ENOTEMPTY != EEXIST)
		// AIX sometimes uses the same value for EEXIST and ENOTEMPTY
		case ENOTEMPTY:
#endif
#ifdef ENOTRECOVERABLE
		case ENOTRECOVERABLE:
#endif
#ifdef ENOTSOCK
		case ENOTSOCK:
#endif
#if defined ENOTSUP && (!defined ENOSYS || ENOTSUP != ENOSYS)
		// zTPF uses the same value for ENOSYS and ENOTSUP
		case ENOTSUP:
#endif
#ifdef ENOTTY
		case ENOTTY:
#endif
#ifdef ENXIO
		case ENXIO:
#endif
#if defined EOPNOTSUPP && (!defined ENOTSUP || EOPNOTSUPP != ENOTSUP)
		case EOPNOTSUPP:
#endif
#ifdef EOVERFLOW
		case EOVERFLOW:
#endif
#ifdef EOWNERDEAD
		case EOWNERDEAD:
#endif
#ifdef EPERM
		case EPERM:
#endif
#ifdef EPIPE
		case EPIPE:
#endif
#ifdef EPROTO
		case EPROTO:
#endif
#ifdef EPROTONOSUPPORT
		case EPROTONOSUPPORT:
#endif
#ifdef EPROTOTYPE
		case EPROTOTYPE:
#endif
		case ERANGE:
#ifdef EROFS
		case EROFS:
#endif
#ifdef ESPIPE
		case ESPIPE:
#endif
#ifdef ESRCH
		case ESRCH:
#endif
#ifdef ETIME
		case ETIME:
#endif
#ifdef ETIMEDOUT
		case ETIMEDOUT:
#endif
#ifdef ETXTBSY
		case ETXTBSY:
#endif
#if defined EWOULDBLOCK && (!defined EAGAIN || EWOULDBLOCK != EAGAIN)
		case EWOULDBLOCK:
#endif
#ifdef EXDEV
		case EXDEV:
#endif
		case 0:
			return util::error_condition(ev, generic_category_instance.obj);

			/* Additional system-dependent mappings from non-standard error codes
			 * to one of the POSIX values above would go here, e.g.
			case EBLAH:
			  return std::error_condition(EINVAL, std::generic_category());
			 */

#endif
		default:
			return util::error_condition(ev, *this);
		}
	}

	// Override this to avoid a virtual call to default_error_condition(i).
	bool equivalent(int i, const util::error_condition &cond) const noexcept final
	{
		return system_error_category::default_error_condition(i) == cond;
	}
};

__constinit constant_init<system_error_category> system_category_instance{};
} // namespace

namespace util
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

error_category::~error_category() = default;

const error_category &system_category() noexcept
{
	return system_category_instance.obj;
}

const error_category &generic_category() noexcept
{
	return generic_category_instance.obj;
}

error_condition error_category::default_error_condition(int __i) const noexcept
{
	return error_condition(__i, *this);
}

bool error_category::equivalent(int __i, const error_condition &__cond) const noexcept
{
	return default_error_condition(__i) == __cond;
}

bool error_category::equivalent(const error_code &__code, int __i) const noexcept
{
	return *this == __code.category() && __code.value() == __i;
}

error_condition error_code::default_error_condition() const noexcept
{
	return category().default_error_condition(value());
}

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace util
