#ifndef PROTOBUF_PROTOBUF_ERROR_HPP
#define PROTOBUF_PROTOBUF_ERROR_HPP

#include "util/system_error.hpp"
#include <cstdint>

namespace protobuf
{

enum class error_code : uint8_t {
	encode_failure = 1,
	decode_failure = 2,
};

util::error_code make_error_code(error_code code);

} // namespace protobuf

// the concrete error code type needs to be made known to the 'system_error'
// implementation that resides in the 'util' namespace
namespace util
{
template <>
struct is_error_code_enum<protobuf::error_code> : public std::true_type {
};
} // namespace util

#endif /* PROTOBUF_PROTOBUF_ERROR_HPP */
