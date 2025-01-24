#ifndef PROTOBUF_PROTOBUF_MESSAGE_HPP
#define PROTOBUF_PROTOBUF_MESSAGE_HPP

#include <zephyr/logging/log.h>
#include "protobuf_error.hpp"
#include "util/system_error.hpp"
#include <expected>
#include <pb_decode.h>
#include <pb_encode.h>
#include <span>

namespace protobuf
{

/**
 * @brief Template class for handling encoding and decoding of a certain protobuf message.
 */
template <typename message_type, size_t maximum_encoded_size_>
class message
{
public:
	static constexpr size_t maximum_encoded_size = maximum_encoded_size_;

	message(pb_msgdesc_s const &message_definition) : message_definition(message_definition)
	{
	}

	/**
	 * @brief Mutable access to the container protobuf message struct.
	 */
	message_type &data()
	{
		return pb_message;
	}

	/**
	 * @brief Constant access to the contained protobuf message struct.
	 */
	message_type &data() const
	{
		return pb_message;
	}

	/**
	 * @brief Encodes the content of the protobuf message into the given buffer.
	 *
	 * @param buffer A span of the buffer that should be used to encode the message into.
	 * @return A span of the same buffer as given as input, but with only the size of the actual
	 *         encoded size. In case of an encoding error, an error_code is returned instead.
	 */
	std::expected<std::span<uint8_t>, util::error_code> encode(std::span<uint8_t> buffer) const
	{
		LOG_MODULE_DECLARE(protobuf_message); // needed for logging in inline functions

		// create output stream and encode the message into it
		pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());
		const auto status = pb_encode(&stream, &message_definition, &pb_message);

		if (!status) {
			LOG_WRN("Encoding failed: %s\n", PB_GET_ERROR(&stream));
			return std::unexpected{error_code::encode_failure};
		}

		return std::span<uint8_t>{buffer.data(), stream.bytes_written};
	}

	/**
	 * @brief Decodes the given buffer into the protobuf message.
	 *
	 * @param buffer A span of the buffer from which the message should be decoded.
	 * @return Error code showing the decoding success or a potential error.
	 */
	util::error_code decode(std::span<uint8_t> buffer)
	{
		LOG_MODULE_DECLARE(protobuf_message);

		// create input stream and decode message from it
		pb_istream_t stream = pb_istream_from_buffer(buffer.data(), buffer.size());
		const auto status = pb_decode(&stream, &message_definition, &pb_message);

		if (!status) {
			LOG_WRN("Decoding failed: %s\n", PB_GET_ERROR(&stream));
			return error_code::decode_failure;
		}
		return {};
	}

private:
	message_type pb_message; ///< The protobuf message as given per template parameter.
	pb_msgdesc_s const &message_definition; ///< The message definition of the protobuf message
						///< (needed for encoding and decoding).
};

} // namespace protobuf

#endif /* PROTOBUF_PROTOBUF_MESSAGE_HPP */
