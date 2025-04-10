#include "protobuf_error.hpp"
#include <zephyr/logging/log.h>

namespace protobuf
{

namespace
{
struct error_category : public util::error_category {
	[[nodiscard]] const char *name() const noexcept override
	{
		return "protobuf_message";
	}

	[[nodiscard]] const char *message(int ev) const noexcept override
	{
		switch (static_cast<error_code>(ev)) {
		case error_code::encode_failure:
			return "Encode failure";
		case error_code::decode_failure:
			return "Decode failure";
		}

		return "Unknown error";
	}
};

const error_category the_error_category{};

} // namespace

util::error_code make_error_code(error_code code)
{
	return {static_cast<int>(code), the_error_category};
}

} // namespace protobuf
